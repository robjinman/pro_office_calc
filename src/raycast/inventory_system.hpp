#ifndef __PROCALC_RAYCAST_INVENTORY_SYSTEM_HPP_
#define __PROCALC_RAYCAST_INVENTORY_SYSTEM_HPP_


#include <functional>
#include <memory>
#include <map>
#include <set>
#include "raycast/system.hpp"
#include "raycast/component.hpp"


enum class CInventoryKind {
  BUCKET,
  COLLECTABLE
};

enum class CBucketKind {
  COUNTER_BUCKET,
  ITEM_BUCKET
};

struct CInventory : public Component {
  CInventory(CInventoryKind kind, entityId_t entityId)
    : Component(entityId, ComponentKind::C_INVENTORY),
      kind(kind) {}

  CInventoryKind kind;
};

typedef std::unique_ptr<CInventory> pCInventory_t;

struct CBucket : public CInventory {
  CBucket(entityId_t entityId, CBucketKind kind, const std::string& collectableType)
    : CInventory(CInventoryKind::BUCKET, entityId),
      bucketKind(kind),
      collectableType(collectableType) {}

  CBucketKind bucketKind;
  std::string collectableType;

  virtual ~CBucket() = 0;
};

typedef std::unique_ptr<CBucket> pCBucket_t;

struct CCounterBucket : public CBucket {
  CCounterBucket(entityId_t entityId, const std::string& collectableType, int capacity)
    : CBucket(entityId, CBucketKind::COUNTER_BUCKET, collectableType),
      capacity(capacity) {}

  int capacity;
  int count = 0;
};

typedef std::unique_ptr<CCounterBucket> pCCounterBucket_t;

struct CItemBucket : public CBucket {
  friend class InventorySystem;

  public:
    CItemBucket(entityId_t entityId, const std::string& collectableType, int capacity)
      : CBucket(entityId, CBucketKind::ITEM_BUCKET, collectableType),
        capacity(capacity) {}

    int capacity;

  private:
    std::map<std::string, entityId_t> items;
};

typedef std::unique_ptr<CItemBucket> pCItemBucket_t;

struct CCollectable : public CInventory {
  CCollectable(entityId_t entityId, const std::string& collectableType)
    : CInventory(CInventoryKind::COLLECTABLE, entityId),
      collectableType(collectableType) {}

  std::string collectableType;
  int value = 1;
  std::string name;
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

struct EBucketItemsChange : public GameEvent {
  EBucketItemsChange(entityId_t entityId, const std::map<std::string, entityId_t>& items,
    int prevCount, int currentCount)
    : GameEvent("bucket_items_change"),
      entityId(entityId),
      items(items),
      prevCount(prevCount),
      currentCount(currentCount) {}

  entityId_t entityId;
  const std::map<std::string, entityId_t>& items;
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

    // Counter buckets only
    int getBucketValue(const std::string& collectableType) const;
    int subtractFromBucket(const std::string& collectableType, int value);

    // Item buckets only
    const std::map<std::string, entityId_t>& getBucketItems(const std::string& collectableType)
      const;
    void removeFromBucket(const std::string& collectableType, const std::string& name);

  private:
    EntityManager& m_entityManager;
    std::map<entityId_t, pCBucket_t> m_buckets;
    std::map<entityId_t, pCCollectable_t> m_collectables;

    std::map<std::string, entityId_t> m_bucketAssignment;

    void addBucket(CInventory* component);
    void addCollectable(CInventory* component);
};


#endif
