package main

import "fmt"

type node interface {
	eval() node
	prettyPrint()
}

// int operations
type add struct {
	l, r node
}

func newAdd(l, r node) *add {
	return &add{l: l, r: r}
}

func (n *add) eval() node {
	// Check if the result of n.l.eval() is an intLit
	lv, ok := n.l.eval().(*intLit)
	if !ok {
		panic("runtime type error, must add on intLit")
	}
	// Check if the result of n.r.eval() is an intLit
	rv, ok := n.r.eval().(*intLit)
	if !ok {
		panic("runtime type error, must add on intLit")
	}
	return newIntLit(lv.val + rv.val)
}

func (n *add) prettyPrint() {
	fmt.Printf("( + ")
	n.l.prettyPrint()
	fmt.Printf(" ")
	n.r.prettyPrint()
	fmt.Printf(")")
}

type intLit struct {
	val int
}

func newIntLit(val int) *intLit {
	return &intLit{val: val}
}

func (n *intLit) eval() node {
	return n
}

func (n *intLit) prettyPrint() {
	fmt.Printf("%d", n.val)
}

// List operations
type concat struct {
	l, r node
}

func newConcat(l, r node) *concat {
	return &concat{l: l, r: r}
}

func (n *concat) eval() node {
	// Check if the result of n.l.eval() is a list
	lv, ok := n.l.eval().(*list)
	if !ok {
		panic("runtime type error, must concat on list")
	}
	// Check if the result of n.r.eval() is a list
	rv, ok := n.r.eval().(*list)
	if !ok {
		panic("runtime type error, must concat on list")
	}

	elms := append(lv.elms, rv.elms...)
	return newList(elms)
}

func (n *concat) prettyPrint() {
	fmt.Printf("( ++ ")
	n.l.prettyPrint()
	fmt.Printf(" ")
	n.r.prettyPrint()
	fmt.Printf(")")
}

type list struct {
	elms []*intLit
}

func newList(elms []*intLit) *list {
	return &list{elms: elms}
}

func (n *list) eval() node {
	return n
}

func (n *list) prettyPrint() {
	fmt.Printf("(")
	for _, v := range n.elms {
		v.prettyPrint()
		fmt.Printf(" ")
	}
	fmt.Printf(")")
}

// Tasks, in order of increasing difficulty. Do as many as you can.

// 1. Implement multiplication. (Create a mult node)

// 2. Implement String() (like pretty print, but return the actual string instead of printing to stdout

// 3. Implement variable assignment
// 		- Create a new node called letin: (letin x 10 n) will bind 10 to the value x in subnode n
//	  - Use a map (map[string]node) to represent an "environment" which will track the seen variables. During eval, as you
//			traverse a (letin x 10 n), add map[x] = 10 before traversing n
//	  - Create a new node, var (var x) which represents a variable. During eval, when you see
//				(var x), perform a lookup in your environment to find the value.
//    - You will need to transform both nodes during eval
//			(letin x 10 n) => n.eval()
//			(var x) => (intLit map[x])

// 4. Implement a function that saves the textual representation of nodes to a file
//		- Lookup packages os, bufio, fmt
//		- Lookup functions os.Create, bufio.NewWriter, fmt.Fprintf
//		- Move this logic into a package variable so you can save multiple nodes to the opened file

// 5. Implement a get operation on lists
//		- Create a new node called get: (get l i) which will return the ith element in a list node l
//	  - If i is greater than the length of l, you should report a runtime error (instead of letting go "crash")

// 6. Implement a sort operation on lists
//		- You will perform the actual sorting during eval on the elms field for list type
//		- You need to createa a sort node that represents this operation in the AST
//		- You can define Less, Len, and Swap for the list type to perform the actual
//			sort during eval

func main() {
	n := newAdd(newAdd(newIntLit(5), newIntLit(10)), newIntLit(20))
	n.prettyPrint()
	fmt.Printf("\n")

	e := n.eval()
	e.prettyPrint()
	fmt.Printf("\n")

	n2 := newConcat(newList([]*intLit{newIntLit(1), newIntLit(2), newIntLit(3)}), newList([]*intLit{newIntLit(4), newIntLit(5), newIntLit(6)}))
	n2.prettyPrint()
	fmt.Printf("\n")

	e2 := n2.eval()
	e2.prettyPrint()
	fmt.Printf("\n")
}
