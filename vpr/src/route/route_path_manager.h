#include "rr_graph_fwd.h"
#include "vtr_assert.h"

#include <set>
#include <list>
#include <vector>

#ifndef _PATH_MANAGER_H
#define _PATH_MANAGER_H

/* Extra path data needed by RCV, seperated from t_heap struct for performance reasons
 * Can be accessed by a pointer, won't be initialized unless needed
 *
 * path_rr: The entire partial path up until the route tree
 * 
 * edge: A list of edges from each node in the partial path to reach the next node
 * 
 * backward_delay: The delay of the partial path plus the path from route tree to source
 * 
 * backward_cong: The congestion estimate of the partial path plus the path from route tree to source */
struct t_heap_path {
    std::vector<RRNodeId> path_rr;
    std::vector<RREdgeId> edge;
    float backward_delay = 0.;
    float backward_cong = 0.;
};

/* A class to manage the extra data required for RCV
 * It manages a set containing all the nodes that currently exist in the route tree
 * This class also manages the extra memory allocation required for the t_heap_path structure */
class PathManager {
    public:
    PathManager(bool init_rcv_structures);
    ~PathManager();

    // Checks if the target node exists in the route tree, or current backwards path variable
    // This is needed for RCV as the non minimum distance pathfinding can lead to illegal loops
    // By keeping a set of the current route tree for a net, as well as checking the current path we can prevent this
    bool node_exists_in_tree(t_heap_path* path_data,
                             RRNodeId& to_node);

    // Insert a node into the current route tree set
    void insert_node(RRNodeId node);

    bool is_enabled();

    // Insert the backwards path back into the main route context traceback
    void insert_backwards_path_into_traceback(t_heap_path* path_data, float cost, float backward_path_cost);

    // Dynamically create a t_heap_path structure to be used in the heap
    void alloc_path_struct(t_heap_path*& tptr);

    // Free the path structure if it's initialized
    void free_path_struct(t_heap_path*& tptr);

    // Move the structure from src to dest
    void move(t_heap_path*& dest, t_heap_path*& src);

    void free_all_memory();

    void empty_heap();

    void empty_route_tree_nodes();

    private:

    // Is RCV enabled and thus route_tree_nodes in use
    bool is_enabled_;

    // This is a list of all currently allocated heap path pointers
    std::vector<t_heap_path*> alloc_list_;

    // A list of freed nodes, to be used where possible to avoid unnecessary news
    std::vector<t_heap_path*> freed_nodes_;

    std::set<RRNodeId> route_tree_nodes_;
};

#endif