#ifndef LS_LEGACY_ARRAY_HPP
#define LS_LEGACY_ARRAY_HPP

#include "../LSValue.hpp"
#include <functional>

namespace ls {

class VM;
class LSLegacyArray;

class Key {
    bool _is_integer;
    int integer;
    std::string string;
public:
    Key(int integer) : _is_integer(true), integer(integer) {}
    Key(std::string s) : _is_integer(false), string(s) {}
    bool isInteger() {
        return _is_integer;
    }
    bool isString() {
        return not _is_integer;
    }
    int getInteger() {
        return integer;
    }
    std::string getString() {
        return string;
    }
    bool operator == (const Key& key) const {
        return _is_integer == key._is_integer && (_is_integer ? integer == key.integer : string == key.string);
    }
    bool operator != (const Key& key) const {
        return not operator == (key);
    }
    size_t hashCode() {
        return _is_integer ? std::hash<int>{}(integer) : std::hash<std::string>{}(string);
    }
};

class Element {
public:
    Key* key;
    int hash;
    bool numeric = false;
    LSValue* value = nullptr;
    Element* next = nullptr;
    Element* prev = nullptr;
    Element* hashNext = nullptr;

    LSValue* keyValue();
    void setValue(LSValue* v) {
        value = v;
    }
};

template <typename T>
class Comparator {
public:
    virtual int compare(T, T) const = 0;
    bool operator() (T i, T j) { return compare(i, j); }
};

class ElementComparator : public Comparator<Element*> {
public:
    static const int SORT_ASC = 1;
    static const int SORT_DESC = 2;

    const int mOrder;

    ElementComparator(int order) : mOrder(order) {}

    int compare(Element* v1, Element* v2) const override {
        if (mOrder == SORT_ASC)
            return *v1->value < *v2->value;
        else if (mOrder == SORT_DESC)
            return *v2->value < *v1->value;
        return 0;
    }
};

class KeyComparator : public Comparator<Element*> {
public:
    static const int SORT_ASC = 1;
    static const int SORT_DESC = 2;

    const int mOrder;

    KeyComparator(int order) : mOrder(order) {}

    int compare(Element* v1, Element* v2) const override;
    int compare(Key* v1, Key* v2) const;
};

class PhpIterator {
    Element* e = nullptr;
public:
    PhpIterator(Element* head) : e(head) {}
    bool hasNext() {
        return e != nullptr;
    }
    LSValue* next() {
        auto v = e->value;
        if (e != nullptr)
            e = e->next;
        return v;
    }
    void remove() {}
};

class ReversedPhpIterator {
    Element* e = nullptr;
public:
    ReversedPhpIterator(Element* end) : e(end) {}
    bool hasNext() {
        return e != nullptr;
    }
    LSValue* next() {
        auto v = e->value;
        if (e != nullptr)
            e = e->prev;
        return v;
    }
    void remove() {}
};

class LSLegacyArray : public LSValue {
private:
	static const int START_CAPACITY = 16;
	static const int MAX_CAPACITY;
    static const int ARRAY_CELL_CREATE_OPERATIONS = 2;
    static const int ARRAY_CELL_ACCESS_OPERATIONS = 2;
	static const int RAM_LIMIT = 1000000;

    VM* vm;
	Element* mHead = nullptr;
	Element* mEnd = nullptr;

	int mIndex = 0;
	int mSize = 0;
	int capacity = 0;

	// Calcul optimisé de la ram utilisée (Optimisé niveau UC, pas niveau RAM)
	int mTotalSize = 0;
	LSValue* mParent = nullptr;

	std::vector<Element*> mTable;

public:

	static const int ASC = 1;
	static const int DESC = 2;
	static const int RANDOM = 3;
	static const int ASC_A = 4;
	static const int DESC_A = 5;
	static const int ASC_K = 6;
	static const int DESC_K = 7;

	LSLegacyArray(VM* vm);
	LSLegacyArray(VM* vm, int capacity);
	LSLegacyArray(VM* vm, LSLegacyArray* phpArray);
	virtual ~LSLegacyArray();

	void initTable(int capacity);
	void growCapacity();
	Element* getHeadElement();

	/**
	 * Retourne le nombre d'éléments dans le tableau
	 *
	 * @return Nombre d'éléments
	 */
	int size();

	/**
	 * Retourne la valeur à pour une clé donnée
	 *
	 * @param key
	 *            Clé dont on veut la valeur
	 * @return Valeur à la clé donnée
	 */
	LSValue* get(Key* key);
    LSValue* get(int key);
    LSValue* get(std::string key);

	/**
	 * Vérifie si une clé se trouve bien dans le tableau
	 *
	 * @param key
	 *            Clé à rechercher
	 * @return True si la clé existe
	 */
	bool containsKey(Key* key);

	/**
	 * Vérifie si le tableau contient une valeur donnée
	 *
	 * @param value
	 *            Valeur à rechercher
	 * @return True si la valeur existe dans le tableau
	 */
	bool contains(LSValue* value);

	/**
	 * Retourne la clé associé à une valeur
	 *
	 * @param value
	 *            Valeur à rechercher
	 * @param pos
	 * @return Clé associée à la valeur ou null si la valeur n'existe pas
	 */
	LSValue* search(LSValue* value, int pos);

    /**
     * Supprime l'élément à l'index et le retourne
     *
     * @param index
     * @return l'élément supprimé
     */
	LSValue* removeIndex(int index);

	/**
	 * Supprime un objet par sa clé
	 *
	 * @param key
	 *            Clé à supprimer
	 */
	void remove(Key* key);

	/**
	 * Trie le tableau
	 *
	 * @param comparator
	 */
	void sort(int comparator);

	/**
	 * Trie le tableau
	 *
	 * @param comparator
	 */
	void sort(ElementComparator comparator);

	/**
	 * Inverse l'ordre
	 */
	void reverse();

	void assocReverse();

	void removeObject(LSValue* value);

	/**
	 * Ajouter un élément à la fin du array
	 *
	 * @param value
	 *            Element à ajouter
	 */
	void push(LSValue* value);

	/**
	 * Ajouter un élément au début du array (décale les index numériques)
	 *
	 * @param value
	 *            Element à ajouter
	 */
	void unshift(LSValue* value);

	/**
	 * Mettre à jour la valeur pour une clé donnée
	 *
	 * @param key
	 *            Clé (Integer, Double ou String)
	 * @param value
	 *            Valeur
	 */
	void set(Key* key, LSValue* value);

	LSValue* getOrCreate(Key* key);

	void set(int key, LSValue* value);

	LSValue* end();

	LSValue* start();

	void insert(int position, LSValue* value);

	// Fonctions "briques de base"

	void reindex();

	void unshiftElement(Element* e);

	void pushElement(Element* e);

	Element* createElement(Key* key, LSValue* value);

	void addToHashMap(Element* e);

	void removeFromHashmap(Element* e);

	void destroyElement(Element* e);

	Element* getElement(Key* key);

	int getHash(Key* key);

	int getIndex(int hash);

	std::string toString() const;

	bool isAssociative();

	bool equals(LSLegacyArray* array);

	void setParent(LSValue* parent);

	void updateArraySize(int delta);

	int getSize();

    virtual bool to_bool() const override;
	virtual bool ls_not() const override;
	virtual bool eq(const LSValue*) const override;
	virtual bool in(const LSValue* const) const override;
	virtual bool in_i(const int) const override;

	int atv(const int key) const;
	virtual LSValue* at(const LSValue* key) const override;
	virtual int abso() const override;

	virtual LSValue* clone() const override;
	virtual std::ostream& dump(std::ostream& os, int level) const override;
	LSValue* getClass(VM* vm) const override;
};

}

#endif