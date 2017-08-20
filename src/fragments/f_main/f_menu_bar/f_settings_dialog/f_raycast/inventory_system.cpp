#include <cassert>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/inventory_system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/entity_manager.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/event_handler_system.hpp"
#include "utils.hpp"


using std::make_pair;
using std::string;
using std::set;


//===========================================
// InventorySystem::update
//===========================================
void InventorySystem::update() {}

//===========================================
// InventorySystem::handleEvent
//===========================================
void InventorySystem::handleEvent(const GameEvent& event) {}

//===========================================
// InventorySystem::handleEvent
//===========================================
void InventorySystem::handleEvent(const GameEvent& event, const set<entityId_t>& entities) {
  if (event.name == "playerMove") {
    for (auto it = m_collectables.begin(); it != m_collectables.end(); ++it) {
      CCollectable& collectable = *it->second;

      if (entities.count(it->first)) {
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
  m_bucketAssignment.insert(make_pair(ptr->type, ptr->entityId()));
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
    m_bucketAssignment.erase(it->second->type);
    m_buckets.erase(it);
  }
  else {
    m_collectables.erase(id);
  }
}

//===========================================
// InventorySystem::addToBucket
//===========================================
void InventorySystem::addToBucket(const CCollectable& item) {
  auto it = m_bucketAssignment.find(item.type);

  if (it != m_bucketAssignment.end()) {
    entityId_t id = it->second;

    CBucket& bucket = *m_buckets.at(id);

    if (bucket.count < bucket.capacity) {
      int prev = bucket.count;
      bucket.count += item.value;

      if (bucket.count > bucket.capacity) {
        bucket.count = bucket.capacity;
      }

      EventHandlerSystem& eventHandlerSystem = m_entityManager
        .system<EventHandlerSystem>(ComponentKind::C_EVENT_HANDLER);

      eventHandlerSystem.sendEvent(id, EBucketCountChange(prev, bucket.count));

      m_entityManager.deleteEntity(item.entityId());
    }
  }
}

//===========================================
// InventorySystem::subtractFromBucket
//===========================================
int InventorySystem::subtractFromBucket(const string& type, int value) {
  auto it = m_bucketAssignment.find(type);
  if (it != m_bucketAssignment.end()) {
    entityId_t id = it->second;
    CBucket& bucket = *m_buckets.at(id);
    int prev = bucket.count;

    if (bucket.count >= value) {
      bucket.count -= value;
    }

    EventHandlerSystem& eventHandlerSystem = m_entityManager
      .system<EventHandlerSystem>(ComponentKind::C_EVENT_HANDLER);

    eventHandlerSystem.sendEvent(id, EBucketCountChange(prev, bucket.count));

    return bucket.count;
  }

  return 0;
}

//===========================================
// InventorySystem::getBucketValue
//===========================================
int InventorySystem::getBucketValue(const string& type) const {
  entityId_t id = m_bucketAssignment.at(type);
  return m_buckets.at(id)->count;
}
