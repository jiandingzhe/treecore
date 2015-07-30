#error "not implemented"
#include "treecore/Object.h"

namespace treecore {

template<typename Type, typename DummyType>
class IntrusiveListAgent
{
    ~IntrusiveListAgent()
    {
        if (prev)
            prev->next = next;
        if (next)
            next->prev = prev;
    }

    IntrusiveListAgent* prev = nullptr;
    IntrusiveListAgent* next = nullptr;
};

template<typename Type, typename DummyType>
class IntrusiveList: public treecore::Object
{
public:
    typedef IntrusiveListAgent<Type, DummyType> AgentType;

    class Iterator
    {
    public:
        Iterator(IntrusiveList& list): ms_agent(list.root)
        {
        }

        Iterator(const Iterator& other): ms_agent(other.ms_agent)
        {
        }

        Iterator(Iterator&& other): ms_agent(other.ms_agent)
        {
            other.ms_agent = nullptr;
        }

        Iterator& operator = (const Iterator& other)
        {
            ms_agent = other.ms_agent;
        }

        Iterator& operator = (Iterator&& other)
        {
            ms_agent = other.ms_agent;
            other.ms_agent = nullptr;
        }

        Type* operator -> () NOEXCEPT
        {
            return ms_agent;
        }

    private:
        AgentType* ms_agent = nullptr;
    };

    IntrusiveList()
    {
        root.prev = &root;
        root.next = &root;
    }

    virtual ~IntrusiveList()
    {
    }

    void append(Type& value) NOEXCEPT
    {
        AgentType* agent = &value;
        AgentType* old_tail = root.prev;

        old_tail->next = agent;
        agent->prev = old_tail;

        agent->next = &root;
        root.prev = agent;
    }

    void prepend(Type& value) NOEXCEPT
    {
        AgentType* agent = &value;
        AgentType* old_head = &root.next;

        agent->next = old_head;
        old_head->prev = agent;

        root->next = agent;
        agent->prev = root;
    }

    Type* begin() NOEXCEPT
    {
        return root.next;
    }

    Type* end() NOEXCEPT
    {
        return &root;
    }

protected:
    AgentType root;
};

}
