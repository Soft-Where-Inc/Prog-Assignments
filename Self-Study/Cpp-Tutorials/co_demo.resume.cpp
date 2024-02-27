/*
 * co_demo.resume.cpp:
 * -----------------------------------------------------------------------------
 * By Alexey Timin.
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
 */
#include <iostream>
#include <experimental/coroutine>

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
        std::experimental::suspend_always await_transform(Event) {
            // you could write code here that adjusted the main
            // program's data structures to ensure the coroutine would
            // be resumed at the right time
            return {};
        }
    };

  private:
    handle_type handle;

    UserFacing(handle_type handle) : handle(handle) {}

  public:
    void resume() {
        handle.resume();
    }
};

UserFacing demo_coroutine() {
    std::cout << "we're about to suspend this coroutine" << std::endl;
    co_await Event{};
    std::cout << "we've successfully resumed the coroutine" << std::endl;
}

int main() {
    UserFacing demo_instance = demo_coroutine();
    std::cout << "we're back in main()" << std::endl;
    demo_instance.resume();
}
