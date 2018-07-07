#include <cassert>
#include "raycast/inventory_system.hpp"
#include "raycast/spatial_system.hpp"
#include "raycast/render_system.hpp"
#include "raycast/entity_manager.hpp"
#include "utils.hpp"
#include "exception.hpp"


using std::make_pair;
using std::string;
using std::set;
using std::map;


//===========================================
// Bucket::~Bucket
//===========================================
Bucket::~Bucket() {}

//===========================================
// InventorySystem::update
//===========================================
void InventorySystem::update() {}

//===========================================
// InventorySystem::handleEvent
//===========================================
void InventorySystem::handleEvent(const GameEvent& event, const set<entityId_t>& entities) {
  // TODO: Should listen for movement of all collectors, not just the player
  if (event.name == "player_move") {
    const EPlayerMove& e = dynamic_cast<const EPlayerMove&>(event);

    for (auto it = m_collectables.begin(); it != m_collectables.end(); ++it) {
      CCollectable& collectable = *it->second;
      entityId_t id = it->first;

      if (entities.count(id)) {
        addToBucket(e.player.body, collectable);
      }
    }
  }
}

//===========================================
// InventorySystem::addComponent
//===========================================
void InventorySystem::addComponent(pComponent_t component) {
  CInventory* ptr = dynamic_cast<CInventory*>(component.release());

  if (ptr->kind == CInventoryKind::COLLECTOR) {
    addCollector(ptr);
  }
  else if (ptr->kind == CInventoryKind::COLLECTABLE) {
    addCollectable(ptr);
  }
}

//===========================================
// InventorySystem::addCollector
//===========================================
void InventorySystem::addCollector(CInventory* component) {
  CCollector* ptr = dynamic_cast<CCollector*>(component);
  assert(ptr != nullptr);

  m_collectors.insert(make_pair(ptr->entityId(), pCCollector_t(ptr)));
}

//===========================================
// InventorySystem::addCollectable
//===========================================
void InventorySystem::addCollectable(CInventory* component) {
  CCollectable* ptr = dynamic_cast<CCollectable*>(component);
  assert(ptr != nullptr);

  m_collectables.insert(make_pair(ptr->entityId(), pCCollectable_t(ptr)));
}

//===========================================
// InventorySystem::hasComponent
//===========================================
bool InventorySystem::hasComponent(entityId_t entityId) const {
  return (m_collectables.find(entityId) != m_collectables.end())
    || (m_collectors.find(entityId) != m_collectors.end());
}

//===========================================
// InventorySystem::getComponent
//===========================================
CInventory& InventorySystem::getComponent(entityId_t entityId) const {
  auto it = m_collectables.find(entityId);
  if (it != m_collectables.end()) {
    return *it->second;
  }

  return *m_collectors.at(entityId);
}

//===========================================
// InventorySystem::removeEntity
//===========================================
void InventorySystem::removeEntity(entityId_t id) {
  auto it = m_collectors.find(id);
  if (it != m_collectors.end()) {
    m_collectors.erase(it);
  }
  else {
    m_collectables.erase(id);
  }
}

//===========================================
// InventorySystem::addToBucket
//
// Countable entities (e.g. ammo) are deleted completely on collection, but items are just moved
// very far away.
//===========================================
void InventorySystem::addToBucket(entityId_t collectorId, const CCollectable& item) {
  auto it = m_collectors.find(collectorId);
  if (it == m_collectors.end()) {
    EXCEPTION("No such collector with ID '" << collectorId << "'");
  }

  CCollector& collector = *it->second;

  auto jt = collector.buckets.find(item.collectableType);
  if (jt == collector.buckets.end()) {
    m_entityManager.fireEvent(ECollectableEncountered{collectorId, item}, { collectorId });
    return;
  }

  Bucket& b = *jt->second;

  switch (b.bucketKind) {
    case BucketKind::COUNTER_BUCKET: {
      CounterBucket& bucket = dynamic_cast<CounterBucket&>(b);

      if (bucket.count < bucket.capacity) {
        int prev = bucket.count;
        bucket.count += item.value;

        if (bucket.count > bucket.capacity) {
          bucket.count = bucket.capacity;
        }

        m_entityManager.fireEvent(EBucketCountChange(collectorId, item.collectableType, bucket,
          prev), { collectorId });

        m_entityManager.deleteEntity(item.entityId());
      }

      break;
    }
    case BucketKind::ITEM_BUCKET: {
      ItemBucket& bucket = dynamic_cast<ItemBucket&>(b);

      if (static_cast<int>(bucket.items.size()) < bucket.capacity) {
        if (contains(bucket.items, item.name)) {
          EXCEPTION("Bucket already contains item with name '" << item.name << "'");
        }

        int prevCount = bucket.items.size();
        bucket.items[item.name] = item.entityId();

        m_entityManager.fireEvent(EBucketItemsChange(collectorId, item.collectableType, bucket,
          prevCount), { collectorId });

        CSpatial& spatial = m_entityManager.getComponent<CSpatial>(item.entityId(),
          ComponentKind::C_SPATIAL);

        if (spatial.kind == CSpatialKind::V_RECT) {
          CVRect& vRect = dynamic_cast<CVRect&>(spatial);
          vRect.pos = Point(10000, 10000);
        }
      }

      break;
    }
  }
}

//===========================================
// InventorySystem::getBucketItems
//===========================================
const map<string, entityId_t>& InventorySystem::getBucketItems(entityId_t collectorId,
  const string& collectableType) const {

  auto it = m_collectors.find(collectorId);
  if (it == m_collectors.end()) {
    EXCEPTION("No such collector with ID '" << collectorId << "'");
  }

  CCollector& collector = *it->second;

  auto jt = collector.buckets.find(collectableType);
  if (jt == collector.buckets.end()) {
    EXCEPTION("Entity does not collect items of type '" << collectableType << "'");
  }

  const Bucket& b = *jt->second;

  if (b.bucketKind != BucketKind::ITEM_BUCKET) {
    EXCEPTION("Cannot retrieve items from bucket; Bucket is not of type ItemBucket");
  }

  const ItemBucket& bucket = dynamic_cast<const ItemBucket&>(b);

  return bucket.items;
}

//===========================================
// InventorySystem::removeFromBucket
//===========================================
void InventorySystem::removeFromBucket(entityId_t collectorId, const string& collectableType,
  const string& name) {

  auto it = m_collectors.find(collectorId);
  if (it == m_collectors.end()) {
    EXCEPTION("No such collector with ID '" << collectorId << "'");
  }

  CCollector& collector = *it->second;

  auto jt = collector.buckets.find(collectableType);
  if (jt == collector.buckets.end()) {
    EXCEPTION("Entity does not collect items of type '" << collectableType << "'");
  }

  Bucket& b = *jt->second;

  if (b.bucketKind != BucketKind::ITEM_BUCKET) {
    EXCEPTION("Cannot remove item from bucket; Bucket is not of type ItemBucket");
  }

  ItemBucket& bucket = dynamic_cast<ItemBucket&>(b);

  int prevCount = bucket.items.size();
  bucket.items.erase(name);

  m_entityManager.fireEvent(EBucketItemsChange(collectorId, collectableType, bucket, prevCount),
    { collectorId });
}

//===========================================
// InventorySystem::subtractFromBucket
//===========================================
int InventorySystem::subtractFromBucket(entityId_t collectorId, const string& collectableType,
  int value) {

  auto it = m_collectors.find(collectorId);
  if (it == m_collectors.end()) {
    EXCEPTION("No such collector with ID '" << collectorId << "'");
  }

  CCollector& collector = *it->second;

  auto jt = collector.buckets.find(collectableType);
  if (jt == collector.buckets.end()) {
    EXCEPTION("Entity does not collect items of type '" << collectableType << "'");
  }

  Bucket& b = *jt->second;

  if (b.bucketKind != BucketKind::COUNTER_BUCKET) {
    EXCEPTION("Cannot subtract from bucket; Bucket is not of type CounterBucket");
  }

  CounterBucket& bucket = dynamic_cast<CounterBucket&>(b);

  int prev = bucket.count;

  if (bucket.count >= value) {
    bucket.count -= value;
  }

  m_entityManager.fireEvent(EBucketCountChange(collectorId, collectableType, bucket, prev),
    { collectorId });

  return bucket.count;
}

//===========================================
// InventorySystem::getBucketValue
//===========================================
int InventorySystem::getBucketValue(entityId_t collectorId, const string& collectableType) const {
  auto it = m_collectors.find(collectorId);
  if (it == m_collectors.end()) {
    EXCEPTION("No such collector with ID '" << collectorId << "'");
  }

  CCollector& collector = *it->second;

  auto jt = collector.buckets.find(collectableType);
  if (jt == collector.buckets.end()) {
    EXCEPTION("Entity does not collect items of type '" << collectableType << "'");
  }

  const Bucket& b = *jt->second;

  if (b.bucketKind != BucketKind::COUNTER_BUCKET) {
    EXCEPTION("Cannot retrieve bucket value; Bucket is not of type CounterBucket");
  }

  const CounterBucket& bucket = dynamic_cast<const CounterBucket&>(b);
  return bucket.count;
}
