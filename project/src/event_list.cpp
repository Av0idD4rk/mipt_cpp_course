#include "event_list.h"

namespace nano_edr {
void EventList::PopFront() {
    if (!head_) return;
    const auto* old = head_;
    head_ = old->next;

    if (!head_)
        tail_ = nullptr;

    --size_;
    delete old;
}

void EventList::Clear() {
    while (head_)
        PopFront();
}
void EventList::PushBack(const Event& event) {
    auto* node = new EventNode(event);

    if (capacity_ != 0) {
        while (size_ >= capacity_)
            PopFront();
    }
    if (tail_)
        tail_->next = node;
    else
        head_ = node;

    tail_ = node;
    ++size_;
}

EventList::~EventList() {
    Clear();
}
}  // namespace nano_edr
