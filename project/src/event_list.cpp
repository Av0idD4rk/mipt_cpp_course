#include "event_list.h"

namespace nano_edr {
void ListPopFront(EventList* list) {
    if (!list || !list->head)
        return;
    EventNode* old = list->head;
    list->head = old->next;

    if (!list->head)
        list->tail = nullptr;

    --list->size;
    delete old;
}

void ListClear(EventList* list) {
    if (!list)
        return;

    while (list->head)
        ListPopFront(list);
}

EventList::~EventList() {
    ListClear(this);
}

void ListPushBack(EventList* list, const Event* event) {
    if (!list || !event)
        return;

    auto* node = new EventNode(*event, nullptr);

    if (list->capacity != 0) {
        while (list->size >= list->capacity)
            ListPopFront(list);
    }

    if (list->tail)
        list->tail->next = node;
    else
        list->head = node;

    list->tail = node;
    ++list->size;
}
}  // namespace nano_edr
