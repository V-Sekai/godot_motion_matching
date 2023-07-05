#pragma once

#include "AABB.h"

#include <godot_cpp/variant/utility_functions.hpp>

#include <godot_cpp/classes/global_constants.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/node_path.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/templates/vector.hpp>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/method_bind.hpp>

using namespace godot;
struct AABBTree : Resource{
    GDCLASS(AABBTree,Resource)
    aabb::Tree bvh{};

    void setup_tree(int dim,float fattening,size_t nb_particles,bool touching_is_overlap){
        bvh.removeAll();
        delete &bvh;
        bvh = aabb::Tree(dim,fattening,nb_particles,touching_is_overlap);
    }

    void insert_particle_at_position(size_t index, PackedFloat32Array position, float radius)
    {
        auto begin = position.ptrw(), end = position.ptrw(); // We use the ptr as iterator.
        end = std::next(end,position.size());
        std::vector<float> pos (begin,end);
        bvh.insertParticle(index,std::move(pos),radius);
    }

    void insert_particle(size_t index,PackedFloat32Array lowerbound, PackedFloat32Array upperbound)
    {
        auto begin = lowerbound.ptrw(), end = lowerbound.ptrw(); // We use the ptr as iterator.
        end = std::next(end,lowerbound.size());
        std::vector<float> lb (begin,end);
        
        begin = upperbound.ptrw(); end = upperbound.ptrw(); // We use the ptr as iterator.
        end = std::next(end,upperbound.size());
        std::vector<float> ub (begin,end);

        bvh.insertParticle(index,std::move(lb),std::move(ub));
    }

    void remove_particle(size_t index){
        bvh.removeParticle(index);
    }
    void remove_all(){
        bvh.removeAll();
    }

    void update_particle_at_position(size_t index, PackedFloat32Array position, float radius,bool always_reinsert)
    {
        auto begin = position.ptrw(), end = position.ptrw(); // We use the ptr as iterator.
        end = std::next(end,position.size());
        std::vector<float> pos (begin,end);
        
        bvh.updateParticle(index,std::move(pos),radius,always_reinsert);
    }
    void update_particle(size_t index,PackedFloat32Array lowerbound, PackedFloat32Array upperbound,bool always_reinsert)
    {
        auto begin = lowerbound.ptrw(), end = lowerbound.ptrw(); // We use the ptr as iterator.
        end = std::next(end,lowerbound.size());
        std::vector<float> lb (begin,end);
        
        begin = upperbound.ptrw(); end = upperbound.ptrw(); // We use the ptr as iterator.
        end = std::next(end,upperbound.size());
        std::vector<float> ub (begin,end);

        bvh.updateParticle(index,std::move(lb),std::move(ub),always_reinsert);
    }

    godot::PackedInt64Array query_index(unsigned int i)
    {
        auto result = bvh.query( i );
        PackedInt64Array r;
        for(auto f : result)
            r.push_back(f);
        return r;
    }

    godot::PackedInt64Array query_bounds(PackedFloat32Array lowerbound,PackedFloat32Array upperbound)
    {
        auto begin = lowerbound.ptrw(), end = lowerbound.ptrw(); // We use the ptr as iterator.
        end = std::next(end,lowerbound.size());
        std::vector<float> lb (begin,end);
        
        begin = upperbound.ptrw(); end = upperbound.ptrw(); // We use the ptr as iterator.
        end = std::next(end,upperbound.size());
        std::vector<float> ub (begin,end);

        aabb::AABB bounds(lb,ub);
        auto result = bvh.query( bounds );
        PackedInt64Array r;
        for(auto f : result)
            r.push_back(f);
        return r;
    }
    
    protected:
    static void _bind_methods(){
        using namespace godot;
        ClassDB::bind_method(D_METHOD("setup_tree","dimensionality","fattening","nb_particles","touching_is_overlap"), &AABBTree::setup_tree);        
        ClassDB::bind_method(D_METHOD("insert_particle_at_position","index","position","radius"), &AABBTree::insert_particle_at_position);
        ClassDB::bind_method(D_METHOD("insert_particle","index","lowerbound","uppderbound"), &AABBTree::insert_particle);
        ClassDB::bind_method(D_METHOD("remove_particle","index"), &AABBTree::remove_particle);
        ClassDB::bind_method(D_METHOD("remove_all"), &AABBTree::remove_all);

        ClassDB::bind_method(D_METHOD("update_particle_at_position","index","position","radius","always_reinsert"), &AABBTree::update_particle_at_position);
        ClassDB::bind_method(D_METHOD("update_particle","index","lowerbound","upperbound","always_reinsert"), &AABBTree::update_particle);

        ClassDB::bind_method(D_METHOD("query_index","index"), &AABBTree::query_index);
        ClassDB::bind_method(D_METHOD("query_bounds","lowerbound","upperbound"), &AABBTree::query_bounds);
    }

};

