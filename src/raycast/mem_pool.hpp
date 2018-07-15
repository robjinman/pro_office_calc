#ifndef __PROCALC_RAYCAST_MEM_POOL_HPP__
#define __PROCALC_RAYCAST_MEM_POOL_HPP__


#include <vector>
#include <memory>
#include <mutex>


template <class T>
using MemPoolCustomDeleter = std::function<void(T*)>;

template <class T>
using MemPoolPtr = std::unique_ptr<T, MemPoolCustomDeleter<T>>;

template <class T>
class MemPool : public std::enable_shared_from_this<MemPool<T>> {
  public:
    MemPool(int initialSize) {
      for (int i = 0; i < initialSize; ++i) {
        m_blocks.push_back(new char[sizeof(T)]);
        m_free.push_back(m_blocks.size() - 1);
        m_used.push_back(false);
      }
    }

    template<typename... Args>
    MemPoolPtr<T> constructObject(Args&&... args) {
      std::unique_lock<std::mutex> lock{m_mutex};

      if (m_free.empty()) {
        m_blocks.push_back(new char[sizeof(T)]);
        m_free.push_back(m_blocks.size() - 1);
        m_used.push_back(false);
      }

      int idx = m_free.back();
      m_free.pop_back();

      T* p = reinterpret_cast<T*>(m_blocks[idx]);
      new(p) T(args...);

      m_used[idx] = true;

      std::weak_ptr<MemPool<T>> poolWkPtr = this->shared_from_this();

      MemPoolCustomDeleter<T> deleter = [poolWkPtr, idx, this](T* ptr) {
        if (poolWkPtr.lock()) {
          std::unique_lock<std::mutex> lock{m_mutex};

          ptr->~T();
          m_used[idx] = false;
          m_free.push_back(idx);
        }
      };

      return MemPoolPtr<T>(p, std::move(deleter));
    }

  private:
    std::vector<char*> m_blocks;
    std::vector<int> m_free;
    std::vector<bool> m_used;

    std::mutex m_mutex;
};


#endif
