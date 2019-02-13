import java.util.*;

import java.util.concurrent.locks.*;

public class Queue {
  private Integer[] buf;
  private int head;
  private int tail;
  private int size;

  private Lock lock = new ReentrantLock();
  private Condition hasItems = lock.newCondition();
  private Condition hasSpace = lock.newCondition();

  public Queue(int size) {
    this.size = size + 1;
    this.buf = new Integer[this.size];
    this.head = 0;
    this.tail = 0;
  }

  public void push(Integer val) throws InterruptedException {
    lock.lock();
    try {
      int next = (this.tail + 1) % this.size;
      while (next == this.head) {
        hasSpace.await();
      }
      this.buf[this.tail] = val;
      this.tail = next; 
      hasItems.signal();
    } finally {
      lock.unlock();
    }
  }

  public Integer pop() throws InterruptedException {
    lock.lock();
    try {
      while (this.head == this.tail) {
        hasItems.await();
      }
      Integer val = this.buf[this.head];
      this.head = (this.head + 1) % this.size;
      hasSpace.signal();
      return val;
    } finally {
      lock.unlock();
    }
  }

  public static void main(String[] args) {
    Queue q = new Queue(10);

    (new Thread() {
      public void run() {
        for (int i = 0; i < 10; i++) {
          try {
            System.out.format("Got %d\n", q.pop());
          } catch (Exception e) {
          }
        }
      }
    }).start();
    
    try {
      Thread.sleep(1000);
    } catch (Exception e) {
    }

    (new Thread() {
      public void run() {
        for (int i = 0; i < 10; i++) {
          try {
            q.push(i);
          } catch (Exception e) {
          }
        }
      }
    }).start(); 
  }
    
}
