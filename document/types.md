*any*
	.class, print, string(), json()

*number<FP>*
	all math
   *casts* : *bool*, *integer*

*integer* = *number<false>*
*bool* = *integer*
	print as true/false

*list<V>*
	size(), map(), iter()
   *list<number>*
		sum(), average(), min(), max()

*map<K,V>* = *list<V>*

*array<V>* = *list<V>*
	push(V), remove(i)

*string<S>* = *list<char>*
	uppercase(), lowercase()
*char* = *string<1>* + *integer*

*interval* = *list<integer>*

*set<V>* = *list<V>*
	add(V)

*object* = *list<any>*

*function* = {
   args = *list<string>*
}
*class*