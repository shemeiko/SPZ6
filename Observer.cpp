#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <functional>
#include <string.h>

// abstract Subscriper
class Subscriber {
public:
    // pure Update definition
    virtual void Update() = 0;
    // virtual destructor definition
    virtual ~Subscriber() = default;
};

// Concrete subscriber
// WordCountSubscriber getting an update on the word count changes
class WordCountSubscriber : public Subscriber {
private:
    // callback function will be called on update
    std::function<void(const std::string&)> callback;
    // the word_count counter
    int word_count;
    // string identifier of subscriber
    std::string name;
public:
    // constructor with takign an identifier
    WordCountSubscriber(const std::string& n) : name(n) { word_count = 0; };
    // constructor with takign an identifier and callback function
    WordCountSubscriber(const std::string& n, std::function<void(const std::string&)> onUpdate) : name(n), callback(onUpdate) { word_count = 0; };
    // Prints an message when the subscriber receives an update
    virtual void Update() override {
        std::string message = name + ": get update [word_count = " + std::to_string(++word_count) + "]\n";
        if (callback) callback(message);
        else std::cout << message;

    };
};

// abstract observer
class Observer {
protected:
    // array of subscribers
    std::vector<std::shared_ptr<Subscriber>> subscribers;
public:
    // destructor
    virtual ~Observer() = default;
    // abstract Subscriber method that adds a subscriber to the array
    virtual void Subscribe(std::shared_ptr<Subscriber>) = 0;
    // abstract Subscriber method that removes a subscriber from the array
    virtual void Unsubscribe(std::shared_ptr<Subscriber>) = 0;
    // abstract Notify method that notifies subscribers when something happens
    virtual void Notify() = 0;
};

// Concrete observer
class WordCountObserver : public Observer {
public:
    // The implemented Subscribe method that adds a new subscriber to the array
    virtual void Subscribe(std::shared_ptr<Subscriber> sub) override {
        // check if subscriber already in the array
        // find function returns an iterator that points to the subscriber
        // if it == subscribers.end(), then subscriber not in the array
        auto it = std::find(subscribers.begin(), subscribers.end(), sub);
        if (it != subscribers.end()) {
            std::cout << "You're subscriber already!\n";
            return;
        }
        
        // Subscriber not in the array, so let's add him
        subscribers.push_back(sub);
    }

    // The implemented Unsubscribe method that removes subscribers from the array
    virtual void Unsubscribe(std::shared_ptr<Subscriber> sub) override {
        // erase function do resizing the array, while remove function only returns an iterator to the end of the "valid" range
        // remove function shift to the right elements that should be removed
        subscribers.erase(
            std::remove(subscribers.begin(), subscribers.end(), sub),
            subscribers.end()
        );
    }

    // The implemented Notify function that notifies every subscribers in the array
    virtual void Notify() override {
        for (auto& sub : subscribers) {
            sub->Update();
        }
    }
};

int main() {
    // Creating memory-safe Observer with two Subscriber instances
    std::shared_ptr<Observer> observer = std::make_shared<WordCountObserver>();
    std::shared_ptr<Subscriber> sub1 = std::make_shared<WordCountSubscriber>("sub1");
    std::shared_ptr<Subscriber> sub2 = std::make_shared<WordCountSubscriber>("sub2");

    observer->Subscribe(sub1); // Subscribe sub1
    observer->Subscribe(sub2); // Subscribe sub2
    observer->Subscribe(sub1); // throws message 'You're subscriber already!'

    observer->Notify(); // invoke subscribers' Update method

    observer->Unsubscribe(sub1); // Unsubscripe sub1
    observer->Notify(); // invoke subscribers' Update method

    observer->Unsubscribe(sub2);

    // update message storage
    std::vector<std::string> resultStorage;

    // create subscriber instance with callback function
    std::shared_ptr<Subscriber> sub 
        = std::make_shared<WordCountSubscriber>(
            "sub", 
            [&resultStorage](const std::string& msg) {
                resultStorage.push_back(msg);
            }
        );
        
    // Subscriber a new subscriber
    observer->Subscribe(sub);

    std::cout << '\n' << "Enter string: ";

    char buffer[100];

    // put std::cin in buffer 
    std::cin.getline(buffer, sizeof(buffer));

    std::cout << "Processing...\n";
    // count words in buffer
    int space = 0;
    for (int i = 0; i < strlen(buffer); ++i) {
        unsigned char ch = static_cast<unsigned char>(buffer[i]);
        if (space == 0 && isalpha(ch)) {
            observer->Notify();
            space = 1;
        } else if (!isalpha(ch)) {
            space = 0;
        }
    }

    // Unsubscribe the subscriber
    observer->Unsubscribe(sub);

    // Prints all output from subscriber's Update method
    std::cout << '\n';
    std::cout << "All output:\n";
    for (auto& result : resultStorage) {
        std::cout << result;
    }
    
    return 0;
}