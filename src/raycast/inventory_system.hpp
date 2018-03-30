#ifndef __PROCALC_RAYCAST_INVENTORY_SYSTEM_HPP_
#define __PROCALC_RAYCAST_INVENTORY_SYSTEM_HPP_


#include <functional>
#include <memory>
#include <map>
#include <set>
#include "raycast/system.hpp"
#include "raycast/component.hpp"


enum class CInventoryKind {
  COLLECTOR,
  COLLECTABLE
};

enum class BucketKind {
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

struct Bucket {
  Bucket(BucketKind kind)
    : bucketKind(kind) {}

  BucketKind bucketKind;

  virtual ~Bucket() = 0;
};

typedef std::unique_ptr<Bucket> pBucket_t;

struct CCollector : public CInventory {
  CCollector(entityId_t entityId)
    : CInventory(CInventoryKind::COLLECTOR, entityId) {}

  std::map<std::string, pBucket_t> buckets;
};

typedef std::unique_ptr<CCollector> pCCollector_t;

struct CCollectable : public CInventory {
  CCollectable(entityId_t entityId, const std::string& collectableType)
    : CInventory(CInventoryKind::COLLECTABLE, entityId),
      collectableType(collectableType) {}

  std::string collectableType;
  int value = 1;
  std::string name;
};

typedef std::unique_ptr<CCollectable> pCCollectable_t;

struct CounterBucket : public Bucket {
  CounterBucket(int capacity)
    : Bucket(BucketKind::COUNTER_BUCKET),
      capacity(capacity) {}

  int capacity;
  int count = 0;
};

typedef std::unique_ptr<CounterBucket> pCounterBucket_t;

struct ItemBucket : public Bucket {
  public:
    ItemBucket(int capacity)
      : Bucket(BucketKind::ITEM_BUCKET),
        capacity(capacity) {}

    int capacity;
    std::map<std::string, entityId_t> items;
};

typedef std::unique_ptr<ItemBucket> pItemBucket_t;

struct EBucketCountChange : public GameEvent {
  EBucketCountChange(entityId_t entityId, const std::string& collectableType,
    const CounterBucket& bucket, int prevCount)
    : GameEvent("bucket_count_change"),
      entityId(entityId),
      collectableType(collectableType),
      bucket(bucket),
      prevCount(prevCount) {}

  entityId_t entityId;
  std::string collectableType;
  const CounterBucket& bucket;
  int prevCount;
};

struct EBucketItemsChange : public GameEvent {
  EBucketItemsChange(entityId_t entityId, const std::string& collectableType,
    const ItemBucket& bucket, int prevCount)
    : GameEvent("bucket_items_change"),
      entityId(entityId),
      collectableType(collectableType),
      bucket(bucket),
      prevCount(prevCount) {}

  entityId_t entityId;
  std::string collectableType;
  const ItemBucket& bucket;
  int prevCount;
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
    CInventory& getComponent(entityId_t entityId) const override;
    void removeEntity(entityId_t id) override;

    void addToBucket(entityId_t collectorId, const CCollectable& item);

    // Counter buckets only
    int getBucketValue(entityId_t collectorId, const std::string& collectableType) const;
    int subtractFromBucket(entityId_t collectorId, const std::string& collectableType, int value);

    // Item buckets only
    const std::map<std::string, entityId_t>& getBucketItems(entityId_t collectorId,
      const std::string& collectableType) const;
    void removeFromBucket(entityId_t collectorId, const std::string& collectableType,
      const std::string& name);

  private:
    EntityManager& m_entityManager;
    std::map<entityId_t, pCCollector_t> m_collectors;
    std::map<entityId_t, pCCollectable_t> m_collectables;

    void addCollector(CInventory* component);
    void addCollectable(CInventory* component);
};


#endif
