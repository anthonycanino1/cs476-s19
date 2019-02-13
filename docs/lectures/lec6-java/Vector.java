import java.util.*;
import java.util.concurrent.*;

public class Vector extends RecursiveAction {
  private int[] a;
  private int[] b;
  private int start;
  private int end;

  public Vector(int[] a, int[] b, int start, int end) {
    this.a = a;
    this.b = b;
    this.start = start;
    this.end = end;
  }

  @Override
  protected void compute() {
    if (end - start < 8) {
      for (int i = start; i < end; i++) {
        b[i] += a[i];
      }
    } else {
      int mid = (start + end) / 2;
      invokeAll(new Vector(a, b, start, mid), new Vector(a, b, mid, end));
    }
  }

  public static void main(String[] args) {
    ForkJoinPool pool = new ForkJoinPool();

    int[] a = new int[1024];
    int[] b = new int[1024];
    for (int i = 0; i < 1024; i++) {
      a[i] = i;
      b[i] = i;
    }

    Vector job = new Vector(a, b, 0, 1024);

    pool.invoke(job);

    for (int i = 0; i < 1024; i++) {
      if (b[i] != a[i] * 2) {
        System.out.format("Failing at %d with %d %d\n", i, b[i], a[i]);
        System.exit(1);
      }
    }

    System.out.println("All good!");

  }

}

