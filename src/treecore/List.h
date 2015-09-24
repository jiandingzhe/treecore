#ifndef TREECORE_LIST_H
#define TREECORE_LIST_H

#include "treecore/RefCountObject.h"

namespace treecore {

template<typename T>
class List: public RefCountObject
{
    struct Node
    {
        Node* prev = nullptr;
        Node* next = nullptr;
        T value = nullptr;
    };


public:
    List()
    {
        ms_root.next = &ms_root;
        ms_root.prev = &ms_root;
    }

    bool append(T value) noexcept
    {
        Node* old_tail = ms_root.prev;

        Node* node = new (std::nothrow) Node{old_tail, &ms_root, value};
        if (!node)
            return false;

        old_tail->next = node;
        ms_root.prev = node;

        return true;
    }

    bool prepend(T value) noexcept
    {
        Node* old_head = ms_root.next;

        Node* node = new (std::nothrow) Node{&ms_root, old_head, value};
        if (!node)
            return false;

        ms_root.next = node;
        old_head->prev = node;

        return true;
    }

    bool pop() noexcept
    {
        if (is_empty())
        {
            return false;
        }
        else
        {
            Node* old_tail = ms_root.prev;
            Node* new_tail = old_tail->prev;

            new_tail->next = &ms_root;
            ms_root.prev = new_tail;

            delete old_tail;
            return true;
        }
    }

    bool shift() noexcept
    {
        if (is_empty())
        {
            return false;
        }
        else
        {
            Node* old_head = ms_root.next;
            Node* new_head = old_head->next;

            new_head->prev = &ms_root;
            ms_root.next = new_head;

            delete old_head;
            return true;
        }
    }

    bool is_empty() noexcept
    {
        return ms_root->prev == ms_root;
    }

private:
    Node ms_root;
};

}

#endif // TREECORE_LIST_H
