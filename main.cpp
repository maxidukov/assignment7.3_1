#include <iostream>
#include <vector>
#include <future>
#include <algorithm>
#include <thread>

template<class Num>
std::size_t my_min_index(const std::vector<Num>&, const std::size_t&);
template<class Num>
void my_min_index_promise(const std::vector<Num>&, const std::size_t&, std::promise<std::size_t>);
template<class Num>
void my_sort(std::vector<Num>&);
template<class Num>
void my_sort_async(std::vector<Num>&);

int main(){
    //std::cout << "Main thread with id " << std::this_thread::get_id() << " launched\n";
    std::vector<int> vec{1,4,3,7,59,-3};
    //std::vector<int> vec{};

    std::cout << "Vector before sorting:\n";
    for(auto& elem : vec )  std::cout << elem << " ";
    std::cout << "\n";
    try{ my_sort_async(vec);}
    catch(const std::exception& e){ std::cout << "Error while sorting: " << e.what(); return 0; }

    std::cout << "Vector after sorting:\n";
    for(auto& elem : vec )  std::cout << elem << " ";
    std::cout << "\n";

    return 0;
}

template<class Num>
std::size_t my_min_index(const std::vector<Num>& vec, const std::size_t& start){
    std::size_t min_index = start;
    for( std::size_t i = start; i < vec.size();i++ ){
        if(vec[i] < vec[min_index]) min_index = i;
    }
    return min_index;
}
template<class Num>
void my_min_index_promise(const std::vector<Num>& vec, const std::size_t& start, std::promise<std::size_t> prm){
    //std::cout << "This is happening in thread " << std::this_thread::get_id() << "\n";
    if(vec.size()) prm.set_value(my_min_index(vec, start));
    //else prm.set_exception(std::make_exception_ptr(std::logic_error("Empty vector!\n")));
}
template<class Num>
void my_sort(std::vector<Num>& vec){
    if(!vec.size()) throw std::logic_error("Empty vector!\n");
    for( std::size_t i = 0; i < vec.size() - 1; i++ )  std::swap(vec[i], vec[my_min_index(vec, i+1)]);
}
template<class Num>
void my_sort_async(std::vector<Num>& vec){
    if(!vec.size()) throw std::logic_error("Empty vector!\n");
    for( std::size_t i = 0; i < vec.size() - 1; i++ ){
        std::promise<std::size_t> prm;
        std::future<std::size_t> ftr = prm.get_future();
        std::future<void> ugly_ftr = std::async(my_min_index_promise<Num>, vec, i, std::move(prm));
        //std::future<void> ugly_ftr = std::async(my_min_index_promise, vec, i, std::move(prm)); // THIS THROWS error: no matching function for call to ‘async(<unresolved overloaded function type>, std::vector<int>&)’
        std::size_t min_index = ftr.get();
        std::swap(vec[i], vec[min_index]);
    }
}
