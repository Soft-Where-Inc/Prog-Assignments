#include <coroutine>
#include <iostream>

struct Event {
    // you could put a description of a specific event in here
};

class UserFacing {
  public:
    class promise_type;
    using handle_type = std::coroutine_handle<promise_type>;
    class promise_type {
      public:
        UserFacing get_return_object() {
            auto handle = handle_type::from_promise(*this);
            return UserFacing{handle};
        }
        std::suspend_never initial_suspend() { return {}; }
        void return_void() {}
        void unhandled_exception() {}
        std::suspend_always final_suspend() noexcept { return {}; }
        std::suspend_always await_transform(Event) {
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
