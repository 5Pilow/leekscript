#ifndef LS_MAP_BASE
#define LS_MAP_BASE

#include "../LSValue.hpp"
#include <map>

namespace ls {

template <typename K>
struct lsmap_less {
	bool operator() (K lhs, K rhs) const;
};

template <typename K, typename V>
class LSMap : public LSValue, public std::map<K, V, lsmap_less<K>> {
public:
	static LSMap<K, V>* constructor();

	LSMap();
	virtual ~LSMap();

	/*
	 * Map methods;
	 */
	static int std_size(const LSMap<K, V>* const map);
	static bool std_insert(LSMap<K, V>* map, K key, V value);
	static void std_emplace(LSMap<K, V>* map, K key, V value);
	static LSMap<K, V>* std_clear(LSMap<K, V>* map);
	static bool std_erase(LSMap<K, V>* map, K key);
	template <class V2>
	static V2 std_look(const LSMap<K, V>* const map, K key, V2 def);
	static V std_max(const LSMap<K, V>* const map);
	static K std_maxKey(const LSMap<K, V>* const map);
	static V std_min(const LSMap<K, V>* const map);
	static K std_minKey(const LSMap<K, V>* const map);
	static LSArray<V>* values(const LSMap<K, V>* const map);
	template <class K2, class V2>
	bool map_equals(const LSMap<K2, V2>* map) const;
	template <class K2, class V2>
	bool map_lt(const LSMap<K2, V2>* map) const;
	template <class F>
	static void std_iter(const LSMap<K, V>* const map, F function);
	static V std_at(const LSMap<K, V>* const map, K key);
	static bool std_in(const LSMap<K, V>* const map, const LSValue*);

	/*
	 * LSValue methods;
	 */
	virtual bool to_bool() const override;
	virtual bool ls_not() const override;

	bool eq(const LSValue*) const override;
	bool lt(const LSValue*) const override;

	V at_k(const K key) const;
	LSValue* at(const LSValue* key) const override;

	virtual LSValue** atL(const LSValue* key) override;
	static V* atL_base(LSMap<K, V>* map, K key);

	bool in_k(K) const;
	bool in(const LSValue*) const override;

	virtual std::ostream& dump(std::ostream&, int level) const override;
	virtual std::string json() const override;
	virtual LSValue* clone() const override;
	virtual LSValue* getClass(VM* vm) const override;
};

}

#ifndef _GLIBCXX_EXPORT_TEMPLATE
#include "LSMap.tcc"
#endif

#endif
