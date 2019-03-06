package main

import "fmt"

type IntNode struct {
	next *IntNode
	val  int
}

func NewIntNode(n *IntNode, v int) *IntNode {
	nn := new(IntNode)
	nn.next = n
	nn.val = v
	return nn
}

type IntLinkedList struct {
	head, tail *IntNode
}

func insert(ll *IntLinkedList, v int) {
	if ll.head == nil {
		ll.head = NewIntNode(nil, v)
		ll.tail = ll.head
	} else {
		ll.tail.next = NewIntNode(nil, v)
		ll.tail = ll.tail.next
	}
}

func main() {
	ll := new(IntLinkedList) // ll = *IntLinkedList
	for i := 0; i < 10; i++ {
		insert(ll, i)
	}

	// always . for selector, never ->
	for n := ll.head; n != nil; n = n.next {
		fmt.Printf("%d ", n.val)
	}
	fmt.Printf("\n")
}
