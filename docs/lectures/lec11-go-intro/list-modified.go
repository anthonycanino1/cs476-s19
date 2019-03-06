package main

import (
	"fmt"
	"sort"
)

type IntNode struct {
	next *IntNode
	val  int
}

func NewIntNode(n *IntNode, v int) *IntNode {
	return &IntNode{next: n, val: v}
}

type IntLinkedList struct {
	head, tail *IntNode
}

func (ll *IntLinkedList) insert(v int) {
	if ll.head == nil {
		ll.head = NewIntNode(nil, v)
		ll.tail = ll.head
	} else {
		ll.tail.next = NewIntNode(nil, v)
		ll.tail = ll.tail.next
	}
}

func (ll *IntLinkedList) traverse(f func(int)) {
	for n := ll.head; n != nil; n = n.next {
		f(n.val)
	}
}

type IntArrayList struct {
	arr []int
}

func (al *IntArrayList) insert(v int) {
	al.arr = append(al.arr, v)
}

func (al *IntArrayList) traverse(f func(int)) {
	for _, v := range al.arr {
		f(v)
	}
}

func (al *IntArrayList) String() (s string) {
	for i, v := range al.arr {
		s += fmt.Sprintf("%d:%d ", i, v)
	}
	return
}

func (al *IntArrayList) Len() int {
	return len(al.arr)
}

func (al *IntArrayList) Less(i, j int) bool {
	return al.arr[i] < al.arr[j]
}

func (al *IntArrayList) Swap(i, j int) {
	al.arr[i], al.arr[j] = al.arr[j], al.arr[i]
}

type IntList interface {
	insert(int)
	traverse(func(int))
}

func populate(il IntList, size int) {
	for i := 0; i < size; i++ {
		il.insert(size - i)
	}
	il.traverse(func(val int) { fmt.Printf("%d ", val) })
	fmt.Printf("\n")
}

func main() {
	ll := new(IntLinkedList) // ll = *IntLinkedList
	insertAndTraverse(ll, 10)

	al := new(IntArrayList) // al = *ArrayLinkedList
	insertAndTraverse(al, 10)

	sort.Sort(al)
	fmt.Printf("%s\n", al)
}
