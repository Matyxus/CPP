#pragma once

#include <vector>
#include <memory>
#include <string>

class state {
private:
    const unsigned int cost;
    const std::shared_ptr<const state> predecessor;

public:
    /**
     * @return Neighbours of current state.
     */
    virtual std::vector<std::shared_ptr<const state>> next_states() const = 0;

    /**
     * @return 'true' if state is goal.
     */
    virtual bool is_goal() const = 0;

    /**
     * @return Path cost leading up to this state from beginning.
     */
    unsigned int current_cost() const {
        return cost;
    }

    /**
     * @return Identifier of current state, must be unique.
     */
    virtual unsigned long long get_identifier() const = 0;

    /**
     * @return Predecessor on path leading to this state.
     */
    std::shared_ptr<const state> get_predecessor() const {
        return predecessor;
    }

    state(const std::shared_ptr<const state> predecessor, unsigned int cost)
            : predecessor(predecessor), cost(cost) {}
    virtual ~state() {}
};

