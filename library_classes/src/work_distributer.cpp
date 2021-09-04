#include "work_distributer.hpp"

Work_Distributer::Work_Distributer(std::vector<std::unique_ptr<Node>>::iterator begin,std::vector<std::unique_ptr<Node>>::iterator end, size_t thread_count){
size_t distance = std::distance(begin, end);

//rng = std::mt19937(rd());


locks = std::vector<std::mutex>(thread_count);
thread_positions = std::vector<size_t>(thread_count, 0);

    thread_limits = {0};

    for(size_t i=0; i<thread_count; i++){
        if(i<(distance % thread_count)){
            thread_limits.push_back(thread_limits[i]+1+(distance/thread_count));
        } else{
            thread_limits.push_back(thread_limits[i]+(distance/thread_count));
        }
    }

    thread_limits_work_copy = thread_limits;
    for(size_t i=0; i<thread_positions.size(); i++){
        thread_positions[i]=thread_limits[i];
    }
}

std::vector<size_t>::iterator Work_Distributer::get_position_ptr(size_t process_id){
    return thread_positions.begin()+process_id;
}

void Work_Distributer::reset_workcopy(){
    thread_limits_work_copy = thread_limits;
}

void Work_Distributer::reset_positions(){
    for(size_t i=0; i<thread_positions.size(); i++){
        thread_positions[i]=thread_limits[i];
    }
}

bool Work_Distributer::next(size_t process_id){
    bool result = false;
    locks[process_id].lock();
    thread_positions[process_id]++;
    if(thread_positions[process_id]<thread_limits_work_copy[process_id+1]){
        result = true;
    }
    locks[process_id].unlock();
    return result;
}
//hat die besten ergebnisse geliefert wenn alle locks rausgenommen hat -> dann ist an die tensorflow queue rangekommen und hatte geringere Zeiten pro Prozess.
bool Work_Distributer::steal(size_t process_id){
    bool result = false;
    int pos = -1;
    for(size_t i = process_id+1; i<thread_positions.size(); i++){
        //locks[i].lock();
        if(thread_positions[i]<thread_limits_work_copy[i+1]-1){//Problem ist, das man schon bei 2 sein kann, und trozdem die 3 klaut -> doppelte bearbeitung. Idee: <thread_limits_work_copy[..]-1 -> wurde dadurch behoben
            locks[i].lock();
            pos = i;
            break;
        }/*else{
            locks[i].unlock();
        }*/
    }
    if(pos>-1){
        result = true;
        thread_positions[process_id] = thread_limits_work_copy[pos+1]-1;
        thread_limits_work_copy[pos+1]--;
        locks[pos].unlock();
    }
    return result;
}
//randomisiert "einfach" die stelle die als nächstes dran wäre klauen