/*
 * co_demo.resume.cpp:
 * -----------------------------------------------------------------------------
 * By Simon Tatham, 2023-08-06
 *
 * Updated commentary and fixed compilation errors, and changes to get the
 * basic program to run on my Mac using these references:
 *
 * Usage: g++ -std=c++20 -o co_demo.resume co_demo.resume.cpp
 *        ./co_demo.resume [test_*]
 *        ./co_demo.resume [--help | test_<something> | test_<prefix> ]
 *        ./co_demo.resume test_coroutines
 *
 * Ref:
 *  [1] https://www.chiark.greenend.org.uk/~sgtatham/quasiblog/coroutines-c++20/
 *      Downloaded from the above extensive write-up building up the
 *      construction of programs with coroutines.
 * -----------------------------------------------------------------------------
 */
#include <iostream>
#include <experimental/coroutine>

// Fabricate a string to track code-location of call-site.
#define LOC() \
    "[" + std::string{__func__} + "():" + std::to_string(__LINE__) + "] "

struct Event {
    // you could put a description of a specific event in here
};

class UserFacing {
  public:
    class promise_type;
    using handle_type = std::experimental::coroutine_handle<promise_type>;
    class promise_type {
      public:
        UserFacing get_return_object() {
            auto handle = handle_type::from_promise(*this);
            return UserFacing{handle};
        }
        std::experimental::suspend_never initial_suspend() { return {}; }
        void return_void() {}
        void unhandled_exception() {}
        std::experimental::suspend_always final_suspend() noexcept { return {}; }

        // co_await on Event{} will invoke this method.
        std::experimental::suspend_always await_transform(Event) {
            // you could write code here that adjusted the main
            // program's data structures to ensure the coroutine would
            // be resumed at the right time
            std::cout << LOC() << "Suspending ...\n";
            return {};
        }
    };

  private:
    handle_type handle;

    UserFacing(handle_type handle) : handle(handle) {}

  public:
    // demo_instance.resume() will invoke this method.
    void resume() {
        std::cout << LOC() << "Resuming ...\n";
        handle.resume();
    }
};

UserFacing demo_coroutine() {
    std::cout << LOC() << "We're about to suspend this coroutine" << std::endl;
    co_await Event{};
    std::cout << LOC() << "We've successfully resumed the coroutine" << std::endl;
}

int main() {

    // demo_instance is an instance of an UserFacing object. It is instantiated
    // by calling the demo_coroutine() function, which will run
    // await_transform(), and gets suspended. The handle returned is the one
    // returned by get_return_object().
    UserFacing demo_instance = demo_coroutine();

    std::cout << LOC() << "We're back in main()" << std::endl;

    // Resume the suspended coroutine by invoking the resume() method in the
    // handle we saved-off previously.
    demo_instance.resume();
}
