# Python code of insert node at begin in
# doubly Circular linked list.
#modulo per gestire i giocatori attraverso una lista concatenata circolare doppia
class Node:
    def __init__(self, player,status, lives):
        self.data = player
        self.next = None
        self.prev = None
        self.status = status
        self.lives = lives


def insertAtBeginning(head, newData, lives, status):
    newNode = Node(newData, status, lives)

    if head is None:

        # List is empty
        newNode.next = newNode.prev = newNode
        head = newNode
    else:

        # List is not empty
        last = head.prev

        # Insert new node
        newNode.next = head
        newNode.prev = last
        head.prev = newNode
        last.next = newNode

        # Update head
        head = newNode

    return head


def printList(head):
    if not head:
        return
    curr = head
    while True:
        print(curr.data, end=" ")
        curr = curr.next
        if curr == head:
            break
    print()


# Linked List : 10<->20<->30

