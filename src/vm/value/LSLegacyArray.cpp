#include "LSLegacyArray.hpp"
#include "../VM.hpp"
#include "LSNull.hpp"
#include "LSNumber.hpp"
#include "LSString.hpp"
#include "../VM.hpp"
#include "../../environment/Environment.hpp"

namespace ls {

LSValue* Element::keyValue() {
	if (key->isInteger())
		return LSNumber::get(key->getInteger());
	else
		return new LSString(key->getString());
}

int KeyComparator::compare(Element* v1, Element* v2) const {
	if (mOrder == SORT_ASC)
		return compare(v1->key, v2->key);
	else if (mOrder == SORT_DESC)
		return compare(v2->key, v1->key);
	return 0;
}

int KeyComparator::compare(Key* v1, Key* v2) const {
	if (v1->isString() && v2->isString())
		return v1->getString() < v2->getString();
	else if (v1->isInteger() && v2->isInteger())
		return v1->getInteger() < v2->getInteger();
	else if (v1->isInteger())
		return -1;
	return 1;
}

const int LSLegacyArray::MAX_CAPACITY = 32000;

LSLegacyArray::LSLegacyArray(VM* vm) : LSValue(INTERVAL), vm(vm) {}

LSLegacyArray::LSLegacyArray(VM* vm, int capacity) : LSValue(INTERVAL), vm(vm) {
	initTable(capacity);
}

LSLegacyArray::LSLegacyArray(VM* vm, LSLegacyArray* phpArray) : LSValue(INTERVAL), vm(vm) {
	if (phpArray->size() > 0) {
		initTable(phpArray->size());
		auto e = phpArray->mHead;
		while (e != nullptr) {
			set(e->key, e->value->clone());
			e = e->next;
		}
	}
}
LSLegacyArray::~LSLegacyArray() {}

void LSLegacyArray::initTable(int capacity) {
	vm->add_operations(capacity / 5);
	this->capacity = capacity;
	mTable.resize(capacity, nullptr);
}

void LSLegacyArray::growCapacity() {

	if (capacity == MAX_CAPACITY) return;

	capacity = std::min(capacity * 2, MAX_CAPACITY);

	// Copy in a new array
	auto newArray = new LSLegacyArray(vm, capacity);
	auto e = mHead;
	while (e != nullptr) {
		newArray->set(e->key, e->value);
		e = e->next;
	}
	// Use the table of this new array
	mTable = newArray->mTable;
	mHead = newArray->mHead;
	mEnd = newArray->mEnd;
	mSize = newArray->mSize;
	mIndex = newArray->mIndex;
}

Element* LSLegacyArray::getHeadElement() {
	return mHead;
}

int LSLegacyArray::size() {
	return mSize;
}

LSValue* LSLegacyArray::get(Key* key) {
	auto e = getElement(key);
	return e == nullptr ? LSNull::get() : e->value;
}

LSValue* LSLegacyArray::get(int key) {
	return get(new Key(key));
}
LSValue* LSLegacyArray::get(std::string key) {
	return get(new Key(key));
}

bool LSLegacyArray::containsKey(Key* key) {
	return getElement(key) != nullptr;
}

bool LSLegacyArray::contains(LSValue* value) {
	auto e = mHead;
	while (e != nullptr) {
		if (*e->value == *value)
			return true;
		e = e->next;
	}
	return false;
}

LSValue* LSLegacyArray::search(LSValue* value, int pos) {
	auto e = mHead;
	int p = 0;
	while (e != nullptr) {
		if (p >= pos && e->value->type == value->type && *e->value == *value) {
			if (e->key->isInteger())
				return LSNumber::get(e->key->getInteger());
			else
				return new LSString(e->key->getString());
		}
		e = e->next;
		p++;
	}
	return LSNull::get();
}

LSValue* LSLegacyArray::removeIndex(int index) {
	if (index >= mSize)
		return LSNull::get();
	auto e = mHead;
	int p = 0;
	while (e != nullptr) {
		if (p == index) {
			remove(e->key);
			reindex();
			return e->value;
		}
		e = e->next;
		p++;
	}
	return LSNull::get();
}

void LSLegacyArray::remove(Key* key) {
	auto e = getElement(key);
	if (e == nullptr)
		return;

	destroyElement(e);
	// Si l'élément existe on l'enleve de la hashmap
	removeFromHashmap(e);
	// Puis on refait le chainage sans l'élément
	if (e->prev == nullptr)
		mHead = e->next;
	else
		e->prev->next = e->next;

	if (e->next == nullptr)
		mEnd = e->prev;
	else
		e->next->prev = e->prev;
}

void LSLegacyArray::sort(int comparator) {
	if (mSize == 0) {
		return;
	}
	// création de la liste
	std::vector<Element*> liste;
	auto elem = mHead;
	while (elem != nullptr) {
		liste.push_back(elem);
		elem = elem->next;
	}
	// Trie de la liste
	if (comparator == RANDOM) {
		// Collections.shuffle(liste);
	} else if (comparator == ASC_K || comparator == DESC_K) {
		std::sort(liste.begin(), liste.end(), KeyComparator(comparator == ASC_K ? ElementComparator::SORT_ASC : ElementComparator::SORT_DESC));
	} else {
		std::sort(liste.begin(), liste.end(), ElementComparator((comparator == ASC || comparator == ASC_A) ? ElementComparator::SORT_ASC : ElementComparator::SORT_DESC));
	}
	// Mise en place de la liste
	mHead = liste.at(0);
	Element* prev = nullptr;
	for (int i = 0; i < liste.size(); i++) {
		auto cur = liste.at(i);
		if (prev != nullptr)
			prev->next = cur;
		cur->prev = prev;
		prev = cur;
	}
	prev->next = nullptr;
	mEnd = prev;
	if (comparator == ASC || comparator == RANDOM || comparator == DESC) {
		reindex();
	}
}

void LSLegacyArray::sort(ElementComparator comparator) {
	if (mSize == 0) {
		return;
	}
	auto e = mHead;
	bool isInOrder = true;
	int value = 0;
	while (e != nullptr) {
		if (!e->key->isInteger() || e->key->getInteger() != value) {
			isInOrder = false;
			break;
		}
		value++;
		e = e->next;
	}

	// création de la liste
	std::vector<Element*> liste;
	auto elem = mHead;
	while (elem != nullptr) {
		liste.push_back(elem);
		elem = elem->next;
	}

	// Tri de la liste
	std::sort(liste.begin(), liste.end(), comparator);

	// Mise en place de la liste
	mHead = liste.at(0);
	Element* prev = nullptr;
	for (int i = 0; i < liste.size(); i++) {
		auto cur = liste.at(i);
		if (prev != nullptr)
			prev->next = cur;
		cur->prev = prev;
		prev = cur;
	}
	prev->next = nullptr;
	mEnd = prev;
	if (isInOrder) {
		reindex();
	}
}

void LSLegacyArray::reverse() {
	if (mSize == 0) {
		return;
	}
	Element* prev = nullptr;
	auto current = mEnd;
	Element* tmp;
	while (current != nullptr) {
		// Fin de liste
		if (prev == nullptr) {
			mHead = current;
		} else {
			prev->next = current;
		}
		tmp = prev;
		prev = current;
		current = current->prev;
		prev->prev = tmp;
	}
	prev->next = nullptr;
	mEnd = prev;

	reindex();
}

void LSLegacyArray::assocReverse() {
	if (mSize == 0) {
		return;
	}
	Element* prev = nullptr;
	auto current = mEnd;
	Element* tmp;
	while (current != nullptr) {
		// Fin de liste
		if (prev == nullptr) {
			mHead = current;
		} else {
			prev->next = current;
		}
		tmp = prev;
		prev = current;
		current = current->prev;
		prev->prev = tmp;
	}
	prev->next = nullptr;
	mEnd = prev;
}

void LSLegacyArray::removeObject(LSValue* value) {
	auto e = mHead;
	while (e != nullptr) {
		if (e->value->type == value->type && *e->value == *value) {
			// On a notre élément à supprimer
			// On l'enleve de la HashMap
			removeFromHashmap(e);
			destroyElement(e);
			// Puis on refait le chainage sans l'élément
			if (e->prev == nullptr)
				mHead = e->next;
			else
				e->prev->next = e->next;

			if (e->next == nullptr)
				mEnd = e->prev;
			else
				e->next->prev = e->prev;
			return;
		}
		e = e->next;
	}
}

void LSLegacyArray::push(LSValue* value) {
	auto key = new Key(mIndex);
	auto e = createElement(key, value);
	pushElement(e);
	mSize++;
	if (mSize > capacity) {
		growCapacity();
	}
}

void LSLegacyArray::unshift(LSValue* value) {
	auto key = new Key(0);
	auto e = createElement(key, value);
	unshiftElement(e);
	reindex();
	mSize++;
	if (mSize > capacity) {
		growCapacity();
	}
}

void LSLegacyArray::set(Key* key, LSValue* value) {
	auto e = getElement(key);
	// Si l'élément n'existe pas on le crée
	if (e == nullptr) {
		e = createElement(key, value);
		pushElement(e);
		mSize++;
		if (mSize > capacity) {
			growCapacity();
		}
	} else {
		e->value = value;
	}
}

LSValue* LSLegacyArray::getOrCreate(Key* key) {
	auto e = getElement(key);
	if (e == nullptr) {
		e = createElement(key, LSNull::get());
		pushElement(e);
		mSize++;
		if (mSize > capacity) {
			growCapacity();
			e = getElement(key);
		}
	}
	return e->value;
}

void LSLegacyArray::set(int key, LSValue* value) {
	set(new Key(key), value);
}

LSValue* LSLegacyArray::end() {
	return mEnd == nullptr ? LSNull::get() : mEnd->value;
}

LSValue* LSLegacyArray::start() {
	return mHead == nullptr ? LSNull::get() : mHead->value;
}

void LSLegacyArray::insert(int position, LSValue* value) {
	if (position < 0) {
		return;
	} else if (position >= mSize) {
		push(value);
	} else if (position == 0) {
		unshift(value);
	} else {
		// On crée notre nouvel élément
		auto e = createElement(new Key(mIndex), value);
		// On va rechercher l'élément avant lequel insérer
		auto i = mHead;
		for (int k = 0; k < position; k++)
			i = i->next;

		// On insert l'élément (normalement ça n'est ni la tête ni la queue)
		e->prev = i->prev;
		e->next = i;
		i->prev->next = e;
		i->prev = e;

		// On réindexe
		reindex();

		mSize++;
		if (mSize > capacity) {
			growCapacity();
		}
	}
}

// Fonctions "briques de base"

void LSLegacyArray::reindex() {
	// Réindexer le tableau (Change l'index de toutes les valeurs
	// numériques)
	int new_index = 0;

	auto e = mHead;
	while (e != nullptr) {
		if (e->numeric) {
			auto new_key = new Key(new_index);
			// Changement de clé
			if (*e->key != *new_key) {
				// On regarde si le hashCode change
				if (new_key->hashCode() != e->hash) {
					removeFromHashmap(e);
					e->hash = new_key->hashCode();
					addToHashMap(e);
				}
				e->key = new_key;
			}
			new_index++;
		}
		e = e->next;
	}
	mIndex = new_index;
}

void LSLegacyArray::unshiftElement(Element* e) {// Ajouter un élément au début
	if (mHead == nullptr) { // Tableau vide
		mHead = e;
		mEnd = e;
	} else { // Ajouter au début
		mHead->prev = e;
		e->next = mHead;
		mHead = e;
	}
}

void LSLegacyArray::pushElement(Element* e) {// Ajouter un élément à la fin
	if (mEnd == nullptr) { // Tableau vide
		mHead = e;
		mEnd = e;
	} else { // Sinon on ajoute à la fin
		mEnd->next = e;
		e->prev = mEnd;
		mEnd = e;
	}
}

Element* LSLegacyArray::createElement(Key* key, LSValue* value) {
	// On crée l'élément
	auto e = new Element();
	e->hash = key->hashCode();
	e->key = key;

	// On ajoute la taille de la clé
	int keySize = 1;

	e->value = value;
	if (key->isInteger()) {
		// On met à jour l'index suivant
		int index = key->getInteger();
		if (index >= mIndex)
			mIndex = index + 1;
		e->numeric = true;
	}

	// On l'ajoute dans la hashmap
	addToHashMap(e);

	int operations = ARRAY_CELL_CREATE_OPERATIONS + (int) std::sqrt(mSize) / 3;
	vm->add_operations(operations);

	return e;
}

void LSLegacyArray::addToHashMap(Element* e) {
	if (not mTable.size()) {
		initTable(START_CAPACITY);
	}
	// Ajouter dans la hashmap
	int index = getIndex(e->hash);
	auto f = mTable[index];
	if (f == nullptr)
		mTable[index] = e;
	else {
		while (f->hashNext != nullptr) {
			f = f->hashNext;
		}
		f->hashNext = e;
	}
}

void LSLegacyArray::removeFromHashmap(Element* e) {
	if (not mTable.size()) {
		return; // nothing to to, empty array
	}
	// Remove from hash hashmap
	int index = getIndex(e->hash);
	auto f = mTable[index];
	if (f == nullptr)
		return;
	else if (f == e) {
		mTable[index] = e->hashNext;
		e->hashNext = nullptr;
	} else {
		while (f->hashNext != e && f->hashNext != nullptr) {
			f = f->hashNext;
		}
		if (f->hashNext == nullptr) {
			return;
		}
		f->hashNext = f->hashNext->hashNext;
		e->hashNext = nullptr;
	}
}

void LSLegacyArray::destroyElement(Element* e) {
	mSize--;
	/*
	Limite de RAM
	if (e->key->isInteger())
		e->value->removeFromTable(1);
	else
		e->value->removeFromTable(((String) e->key).length());
	*/
}

Element* LSLegacyArray::getElement(Key* key) {
	if (not mTable.size()) {
		return nullptr; // empty array
	}
	int operations = ARRAY_CELL_ACCESS_OPERATIONS;
	vm->add_operations(operations);

	int hash = getHash(key);
	int index = getIndex(hash);

	auto f = mTable[index];

	while (f != nullptr) {
		if (f->hash == hash && *f->key == *key) {
			return f;
		}
		f = f->hashNext;
	}
	return nullptr;
}

int LSLegacyArray::getHash(Key* key) {
	return key->hashCode();
}

int LSLegacyArray::getIndex(int hash) {
	return hash & (capacity - 1);
}

std::string LSLegacyArray::toString() const {

	vm->add_operations(1 + mSize * 2);

	std::string sb;
	// On va regarder si le tableau est dans l'ordre
	auto e = mHead;

	bool isInOrder = true;
	int value = 0;
	while (e != nullptr) {
		if (!e->key->isInteger() || e->key->getInteger() != value) {
			isInOrder = false;
			break;
		}
		value++;
		e = e->next;
	}

	sb.append("[");
	e = mHead;
	while (e != nullptr) {
		if (e != mHead)
			sb.append(", ");
		if (!isInOrder) {
			if (e->key->isInteger()) {
				sb.append(std::to_string(e->key->getInteger()));
			 } else {
				sb.append('\'' + e->key->getString() + '\'');
			 }
			sb.append(" => ");
		}
		sb.append(e->value->to_string());
		e = e->next;
	}
	sb.append("]");
	return sb;
}

bool LSLegacyArray::isAssociative() {
	auto e = mHead;
	int value = 0;
	while (e != nullptr) {
		if (!e->key->isInteger() || e->key->getInteger() != value) {
			return true;
		}
		value++;
		e = e->next;
	}
	return false;
}

bool LSLegacyArray::equals(LSLegacyArray* array) {

	vm->add_operations(1);

	// On commence par vérifier la taille
	if (mSize != array->mSize)
		return false;
	if (mSize == 0)
		return true;

	vm->add_operations(mSize);

	auto e1 = mHead;
	auto e2 = array->mHead;
	// On va comparer chaque élément 1 à 1
	while (e1 != nullptr) {
		if (*e1->key != *e2->key)
			return false;
		if (*e1->value != *e2->value)
			return false;
		e1 = e1->next;
		e2 = e2->next;
	}
	return true;
}

void LSLegacyArray::setParent(LSValue* parent) {
	mParent = parent;
}

void LSLegacyArray::updateArraySize(int delta) {
	if (delta == 0) {
		return;
	}
	mTotalSize += delta;
	if (mTotalSize >= RAM_LIMIT) {
		// throw new LeekRunException(LeekRunException.OUT_OF_MEMORY);
	}
	if (mParent != nullptr) {
		// mParent->updateSize(delta);
	}
}

int LSLegacyArray::getSize() {
	return mTotalSize;
}

bool LSLegacyArray::to_bool() const {
	return mSize > 0;
}

bool LSLegacyArray::ls_not() const {
	auto r = mSize == 0;
	LSValue::delete_temporary(this);
	return r;
}

bool LSLegacyArray::eq(const LSValue* v) const {
	assert(false);
}

bool LSLegacyArray::in(const LSValue* const value) const {
	assert(false);
}

bool LSLegacyArray::in_i(const int value) const {
	assert(false);
}

int LSLegacyArray::atv(const int key) const {
	assert(false);
}

LSValue* LSLegacyArray::at(const LSValue* key) const {
	assert(false);
}

int LSLegacyArray::abso() const {
	return mSize;
}

LSValue* LSLegacyArray::clone() const {
	assert(false);
}

std::ostream& LSLegacyArray::dump(std::ostream& os, int) const {
	os << toString();
	return os;
}

LSValue* LSLegacyArray::getClass(VM* vm) const {
	return vm->env.legacy_array_class.get();
}

}