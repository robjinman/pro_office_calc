#ifndef __PROCALC_RAYCAST_INVENTORY_SYSTEM_HPP_
#define __PROCALC_RAYCAST_INVENTORY_SYSTEM_HPP_


#include <functional>
#include <memory>
#include <map>
#include "raycast/system.hpp"
#include "raycast/component.hpp"


enum class CInventoryKind {
  BUCKET,
  COLLECTABLE
};

struct CInventory : public Component {
  CInventory(CInventoryKind kind, entityId_t entityId)
    : Component(entityId, ComponentKind::C_INVENTORY),
      kind(kind) {}

  CInventoryKind kind;
};

typedef std::unique_ptr<CInventory> pCInventory_t;

struct CBucket : public CInventory {
  CBucket(entityId_t entityId, const std::string& type, int capacity)
    : CInventory(CInventoryKind::BUCKET, entityId),
      type(type),
      capacity(capacity) {}

  std::string type;
  int capacity;
  int count = 0;
};

typedef std::unique_ptr<CBucket> pCBucket_t;

struct CCollectable : public CInventory {
  CCollectable(entityId_t entityId, const std::string& type, int value)
    : CInventory(CInventoryKind::COLLECTABLE, entityId),
      type(type),
      value(value) {}

  std::string type;
  int value;
};

typedef std::unique_ptr<CCollectable> pCCollectable_t;

struct EBucketCountChange : public GameEvent {
  EBucketCountChange(entityId_t entityId, int prevCount, int currentCount)
    : GameEvent("bucket_count_change"),
      entityId(entityId),
      prevCount(prevCount),
      currentCount(currentCount) {}

  entityId_t entityId;
  int prevCount;
  int currentCount;
};

class EntityManager;

class InventorySystem : public System {
  public:
    InventorySystem(EntityManager& entityManager)
      : m_entityManager(entityManager) {}

    void update() override;
    void handleEvent(const GameEvent& event) override {};
    void handleEvent(const GameEvent& event, const std::set<entityId_t>& entities) override;

    void addComponent(pComponent_t component) override;
    bool hasComponent(entityId_t entityId) const override;
    Component& getComponent(entityId_t entityId) const override;
    void removeEntity(entityId_t id) override;

    void addToBucket(const CCollectable& item);
    int getBucketValue(const std::string& type) const;
    int subtractFromBucket(const std::string& type, int value);

  private:
    EntityManager& m_entityManager;
    std::map<entityId_t, pCBucket_t> m_buckets;
    std::map<entityId_t, pCCollectable_t> m_collectables;

    std::map<std::string, entityId_t> m_bucketAssignment;

    void addBucket(CInventory* component);
    void addCollectable(CInventory* component);
};


#endif
