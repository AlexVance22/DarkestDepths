#include <typeinfo>


template<typename F, typename ... Ts>
struct EnumHelper {
    static const size_t size = sizeof(F) > EnumHelper<Ts...>::size ? sizeof(F) : EnumHelper<Ts...>::size;

    inline static void destroy(size_t id, void * data) {
        if (id == typeid(F).hash_code()) {
            reinterpret_cast<F*>(data)->~F();
        } else {
            EnumHelper<Ts...>::destroy(id, data);
        }
    }
    inline static void copy(size_t old_t, const void* old_v, void* new_v)
    {
        if (old_t == typeid(F).hash_code()) {
            new (new_v) F(*reinterpret_cast<const F*>(old_v));
        } else {
            EnumHelper<Ts...>::copy(old_t, old_v, new_v);
        }
    }
};

template<typename F>
struct EnumHelper<F>  {
    static const size_t size = sizeof(F);

    inline static void destroy(size_t id, void* data) {
        if (id == typeid(F).hash_code()) {
            reinterpret_cast<F*>(data)->~F();
        }
    }
    inline static void copy(size_t old_t, const void* old_v, void* new_v) {
        if (old_t == typeid(F).hash_code()) {
            new(new_v) F(*reinterpret_cast<const F*>(old_v));
        }
    }
};


template<typename ... Ts>
class Enum
{
private:
    using Helper = EnumHelper<Ts...>;

    size_t type_id;
    char data[Helper::size];

    static inline size_t invalid_type() {
        return typeid(void).hash_code();
    }

public:
    Enum(): type_id(invalid_type()) {}
    Enum(Enum<Ts...>&& old): type_id(old.type_id), data(old.data) {
        old.type_id = invalid_type();
    }
    Enum(const Enum<Ts...>& old): type_id(old.type_id) {
        Helper::copy(old.type_id, &old.data, &data);
    }
    ~Enum() { Helper::destroy(type_id, &data); }

    Enum<Ts...>& operator=(Enum<Ts...>&& old) {
        data = old.data;
        old.type_id = invalid_type();
        return *this;
    }
    Enum<Ts...>& operator=(Enum<Ts...> old) {
        std::swap(data, old.data);
        std::swap(type_id, old.type_id);
        return *this;
    }

    template<typename T, typename ... Args>
    void set(Args&&... args) {
        Helper::destroy(type_id, &data);
        new (&data) T(std::forward<Args>(args)...);
        type_id = typeid(T).hash_code();
    }
    template<typename T>
    const T& get() const {
        if (type_id == typeid(T).hash_code()) {
            return *reinterpret_cast<T*>(&data);
        } else {
            throw std::bad_cast();
        }
    }
    template<typename T>
    T& get_mut() {
        if (type_id == typeid(T).hash_code()) {
            return *reinterpret_cast<T*>(&data);
        } else {
            throw std::bad_cast();
        }
    }

    template<typename T>
    bool is() const {
        return (type_id == typeid(T).hash_code());
    }
    bool valid() const {
        return (type_id != invalid_type());
    }
};

