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
// CBucket::~CBucket
//===========================================
CBucket::~CBucket() {}

//===========================================
// InventorySystem::update
//===========================================
void InventorySystem::update() {}

//===========================================
// InventorySystem::handleEvent
//===========================================
void InventorySystem::handleEvent(const GameEvent& event, const set<entityId_t>& entities) {
  if (event.name == "player_move") {
    for (auto it = m_collectables.begin(); it != m_collectables.end(); ++it) {
      CCollectable& collectable = *it->second;
      entityId_t id = it->first;

      if (entities.count(id)) {
        addToBucket(collectable);
      }
    }
  }
}

//===========================================
// InventorySystem::addComponent
//===========================================
void InventorySystem::addComponent(pComponent_t component) {
  CInventory* ptr = dynamic_cast<CInventory*>(component.release());

  if (ptr->kind == CInventoryKind::BUCKET) {
    addBucket(ptr);
  }
  else if (ptr->kind == CInventoryKind::COLLECTABLE) {
    addCollectable(ptr);
  }
}

//===========================================
// InventorySystem::addBucket
//===========================================
void InventorySystem::addBucket(CInventory* component) {
  CBucket* ptr = dynamic_cast<CBucket*>(component);
  assert(ptr != nullptr);

  m_buckets.insert(make_pair(ptr->entityId(), pCBucket_t(ptr)));
  m_bucketAssignment.insert(make_pair(ptr->collectableType, ptr->entityId()));
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
    || (m_buckets.find(entityId) != m_buckets.end());
}

//===========================================
// InventorySystem::getComponent
//===========================================
Component& InventorySystem::getComponent(entityId_t entityId) const {
  auto it = m_collectables.find(entityId);
  if (it != m_collectables.end()) {
    return *it->second;
  }

  return *m_buckets.at(entityId);
}

//===========================================
// InventorySystem::removeEntity
//===========================================
void InventorySystem::removeEntity(entityId_t id) {
  auto it = m_buckets.find(id);
  if (it != m_buckets.end()) {
    m_bucketAssignment.erase(it->second->collectableType);
    m_buckets.erase(it);
  }
  else {
    m_collectables.erase(id);
  }
}

//===========================================
// InventorySystem::addToBucket
//
// Countable entities (e.g. ammo) are deleted completely on collection, but items are only removed
// from the spatial system and the render system. Use the event handler system to listen for
// bucket_items_change events to remove from other systems.
//===========================================
void InventorySystem::addToBucket(const CCollectable& item) {
  auto it = m_bucketAssignment.find(item.collectableType);

  if (it == m_bucketAssignment.end()) {
    EXCEPTION("No such bucket for collectable of type '" << item.collectableType << "'");
  }

  entityId_t id = it->second;

  CBucket& b = *m_buckets.at(id);

  switch (b.bucketKind) {
    case CBucketKind::COUNTER_BUCKET: {
      CCounterBucket& bucket = dynamic_cast<CCounterBucket&>(b);

      if (bucket.count < bucket.capacity) {
        int prev = bucket.count;
        bucket.count += item.value;

        if (bucket.count > bucket.capacity) {
          bucket.count = bucket.capacity;
        }

        m_entityManager.broadcastEvent(EBucketCountChange(id, prev, bucket.count));
        m_entityManager.deleteEntity(item.entityId());
      }

      break;
    }
    case CBucketKind::ITEM_BUCKET: {
      CItemBucket& bucket = dynamic_cast<CItemBucket&>(b);

      if (static_cast<int>(bucket.items.size()) < bucket.capacity) {
        if (contains(bucket.items, item.name)) {
          EXCEPTION("Bucket already contains item with name '" << item.name << "'");
        }

        int prevCount = bucket.items.size();
        bucket.items[item.name] = item.entityId();

        m_entityManager.broadcastEvent(EBucketItemsChange(id, bucket.items, prevCount,
          bucket.items.size()));

        auto& spatialSystem = m_entityManager.system<SpatialSystem>(ComponentKind::C_SPATIAL);
        auto& renderSystem = m_entityManager.system<RenderSystem>(ComponentKind::C_RENDER);

        spatialSystem.removeEntity(item.entityId());
        renderSystem.removeEntity(item.entityId());
      }

      break;
    }
  }
}

//===========================================
// InventorySystem::getBucketItems
//===========================================
const map<string, entityId_t>& InventorySystem::getBucketItems(const string& collectableType)
  const {

  auto it = m_bucketAssignment.find(collectableType);

  if (it == m_bucketAssignment.end()) {
    EXCEPTION("No such bucket for collectable of type '" << collectableType << "'");
  }

  entityId_t id = it->second;
  CBucket& b = *m_buckets.at(id);

  if (b.bucketKind != CBucketKind::ITEM_BUCKET) {
    EXCEPTION("Cannot retrieve items from bucket; Bucket is not of type CItemBucket");
  }

  const CItemBucket& bucket = dynamic_cast<CItemBucket&>(b);

  return bucket.items;
}

//===========================================
// InventorySystem::removeFromBucket
//===========================================
void InventorySystem::removeFromBucket(const string& collectableType, const string& name) {
  auto it = m_bucketAssignment.find(collectableType);

  if (it == m_bucketAssignment.end()) {
    EXCEPTION("No such bucket for collectable of type '" << collectableType << "'");
  }

  entityId_t id = it->second;
  CBucket& b = *m_buckets.at(id);

  if (b.bucketKind != CBucketKind::ITEM_BUCKET) {
    EXCEPTION("Cannot remove item from bucket; Bucket is not of type CItemBucket");
  }

  CItemBucket& bucket = dynamic_cast<CItemBucket&>(b);

  int prevCount = bucket.items.size();
  bucket.items.erase(name);

  m_entityManager.broadcastEvent(EBucketItemsChange(id, bucket.items, prevCount,
    bucket.items.size()));
}

//===========================================
// InventorySystem::subtractFromBucket
//===========================================
int InventorySystem::subtractFromBucket(const string& collectableType, int value) {
  auto it = m_bucketAssignment.find(collectableType);

  if (it == m_bucketAssignment.end()) {
    EXCEPTION("No such bucket for collectable of type '" << collectableType << "'");
  }

  entityId_t id = it->second;
  CBucket& b = *m_buckets.at(id);

  if (b.bucketKind != CBucketKind::COUNTER_BUCKET) {
    EXCEPTION("Cannot subtract value from bucket; Bucket is not of type CCounterBucket");
  }

  CCounterBucket& bucket = dynamic_cast<CCounterBucket&>(b);
  int prev = bucket.count;

  if (bucket.count >= value) {
    bucket.count -= value;
  }

  m_entityManager.broadcastEvent(EBucketCountChange(id, prev, bucket.count));

  return bucket.count;
}

//===========================================
// InventorySystem::getBucketValue
//===========================================
int InventorySystem::getBucketValue(const string& collectableType) const {
  entityId_t id = m_bucketAssignment.at(collectableType);
  const CBucket& b = *m_buckets.at(id);

  if (b.bucketKind != CBucketKind::COUNTER_BUCKET) {
    EXCEPTION("Cannot retrieve value from bucket; Bucket is not of type CCounterBucket");
  }

  const CCounterBucket& bucket = dynamic_cast<const CCounterBucket&>(b);
  return bucket.count;
}
