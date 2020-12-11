// A thread safe queue to be used for messages and other types
#pragma once

#include <eventsocketcpp/RedBackCommon.h> 


namespace RedBack{

    template<typename T>
    class TSQueue {
    
    public:
        TSQueue() = default;

        // Prevent copying because of the use of threading and mutex
        TSQueue(const TSQueue<T>&) = delete;
        
        virtual ~TSQueue() { clear(); }

        // Returns the front element
        const T& front(){
            std::lock_guard<std::mutex> lock{mux};
            return queue.front();
        }


        // Returns the back element
        const T& back(){
            std::lock_guard<std::mutex> lock{mux};
            return queue.back();
        }

        // Place an element at the back
        void push_back(const T& t){
            std::lock_guard<std::mutex> lock{mux};
            queue.emplace_back(std::move(t));

            std::unique_lock<std::mutex> ul(blockingMutex);
            cvBlocking.notify_one();
        }

        // Place an element at the front
        void push_front(const T& t){
            std::lock_guard<std::mutex> lock{mux};
            queue.emplace_front(std::move(t));

            std::unique_lock<std::mutex> ul(blockingMutex);
            cvBlocking.notify_one();
        }

        // Remove an element from the back
        void pop_back(){
            std::lock_guard<std::mutex> lock{mux};
            queue.pop_back();
        }

        // Remove an element from the front
        void pop_front(){
            std::lock_guard<std::mutex> lock{mux};
            queue.pop_front();
        }

        // Clear the queue
        void clear(){
            std::lock_guard<std::mutex> lock{mux};
            queue.clear();
        }

        // Check if it is empty
        bool isEmpty(){
            std::lock_guard<std::mutex> lock{mux};
            return queue.empty();
        } 

        // Wait until the queue is not empty
        void wait()
        {
            while(isEmpty())
            {
                std::unique_lock<std::mutex> ul(blockingMutex);
                cvBlocking.wait(ul);
            }
        }
    private:
        std::mutex mux;
        std::deque<T> queue;

        std::condition_variable cvBlocking;
        std::mutex blockingMutex;
    };

} // RedBack