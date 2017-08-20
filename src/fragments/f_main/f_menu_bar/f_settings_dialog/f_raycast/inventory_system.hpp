#ifndef __PROCALC_FRAGMENTS_F_RAYCAST_INVENTORY_SYSTEM_HPP_
#define __PROCALC_FRAGMENTS_F_RAYCAST_INVENTORY_SYSTEM_HPP_


#include <functional>
#include <memory>
#include <map>
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/system.hpp"
#include "fragments/f_main/f_menu_bar/f_settings_dialog/f_raycast/component.hpp"


enum class CInventoryKind {
  BUCKET,
  COLLECTABLE
};

struct CInventory : public Component {
  CInventory(CInventoryKind kind, entityId_t entityId)
    : Component(entityId, ComponentKind::C_INVENTORY),
      kind(kind) {}

  CInventoryKind kind;

  virtual ~CInventory() override {}
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

  virtual ~CBucket() override {}
};

typedef std::unique_ptr<CBucket> pCBucket_t;

struct CCollectable : public CInventory {
  CCollectable(entityId_t entityId, const std::string& type, int value)
    : CInventory(CInventoryKind::COLLECTABLE, entityId),
      type(type),
      value(value) {}

  std::string type;
  int value;

  virtual ~CCollectable() override {}
};

typedef std::unique_ptr<CCollectable> pCCollectable_t;

struct EBucketCountChange : public GameEvent {
  EBucketCountChange(int prevCount, int currentCount)
    : GameEvent("bucketCountChange"),
      prevCount(prevCount),
      currentCount(currentCount) {}

  int prevCount;
  int currentCount;
};

class EntityManager;

class InventorySystem : public System {
  public:
    InventorySystem(EntityManager& entityManager)
      : m_entityManager(entityManager) {}

    virtual void update() override;
    virtual void handleEvent(const GameEvent& event) override;
    virtual void handleEvent(const GameEvent& event, const std::set<entityId_t>& entities) override;

    virtual void addComponent(pComponent_t component) override;
    virtual bool hasComponent(entityId_t entityId) const override;
    virtual Component& getComponent(entityId_t entityId) const override;
    virtual void removeEntity(entityId_t id) override;

    void addToBucket(const CCollectable& item);
    int getBucketValue(const std::string& type) const;
    int subtractFromBucket(const std::string& type, int value);

    virtual ~InventorySystem() override {}

  private:
    EntityManager& m_entityManager;
    std::map<entityId_t, pCBucket_t> m_buckets;
    std::map<entityId_t, pCCollectable_t> m_collectables;

    std::map<std::string, entityId_t> m_bucketAssignment;

    void addBucket(CInventory* component);
    void addCollectable(CInventory* component);
};


#endif
