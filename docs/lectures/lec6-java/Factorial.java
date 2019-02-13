import java.util.*;

public class Factorial {
  private long fact = 1;
  private long curr = 0;

  public class Result {
    public long f;
    public long c;
    public Result(long f, long c) {
      this.f = f;
      this.c = c;
    }
  }

  public synchronized Result next() {
    Result r = new Result(fact,curr);
    curr++;
    fact *= curr;
    return r;
  }

  public static void main(String[] args) {
    Factorial f = new Factorial();

    Thread t = new Thread() {
      @Override
      public void run() {
        for (int i = 0; i < 10; i++) {
          Result r = f.next();
          System.out.format("fact(%d) = %d\n", r.c, r.f);
        }
      }
    };
    t.start();

    Thread t2 = new Thread() {
      @Override
      public void run() {
        for (int i = 0; i < 10; i++) {
          Result r = f.next();
          System.out.format("fact(%d) = %d\n", r.c, r.f);
        }
      }
    };
    t2.start();

  }

}
