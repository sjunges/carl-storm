/**
 * @file Singleton.h
 * @author Gereon Kremer <gereon.kremer@cs.rwth-aachen.de>
 */

#pragma once

#include <memory>
#include <mutex>

/**
 * Marks a class or function as exported from libcarl, independent of the -fvisibility setting of the library using it.
 */
#define CARL_EXPORT __attribute__((visibility("default")))

/**
 * Defines getInstance() of a non-template singleton class.
 * Must be used in exactly one source file of libcarl, the class must declare `static T& getInstance();`.
 */
#define CARL_DEFINE_SINGLETON_INSTANCE(T) \
    T& T::getInstance() {                 \
        static T instance;                \
        return instance;                  \
    }

namespace carl {

/**
 * Base class for a singleton.
 *
 * A class that shall be a singleton can inherit from this class (the template argument being the class itself, see CRTP for this).
 * It takes care of
 * <ul>
 * <li>deleting the copy constructor and the assignment operator,</li>
 * <li>providing a protected default constructor and a virtual destructor.</li>
 * </ul>
 *
 * Deliberately, this class does not provide getInstance()  to avoid issues with hidden visibility or LTO.
 * Instead, a non-template singleton class T must declare `static T& getInstance();` and define it in a source file via CARL_DEFINE_SINGLETON_INSTANCE(T).
 * Class templates use `InlineSingleton` instead.
 */
template<typename T>
class CARL_EXPORT Singleton {
   protected:
    /**
     * Protected default constructor.
     */
    Singleton() = default;

   public:
    Singleton(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton& operator=(Singleton&&) = delete;

    /**
     * Virtual destructor.
     */
    virtual ~Singleton() noexcept = default;
};

/**
 * Singleton with an inline getInstance(), only for class templates.
 *
 * Warning: uniqueness of the instance across shared libraries is not guaranteed,
 * as each library instantiating getInstance() contains its own copy of the instance.
 * Use Singleton for non-template classes.
 */
template<typename T>
class CARL_EXPORT InlineSingleton : public Singleton<T> {
   protected:
    InlineSingleton() = default;

   public:
    /**
     * Returns the single instance of this class by reference.
     * If there is no instance yet, a new one is created.
     */
    static T& getInstance() {
        static T t;
        return t;
    }
};

}  // namespace carl
