import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.Reader;
import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.Locale;
import java.util.StringTokenizer;

public class condVariance {

    private class State {
        final int x;
        final ArrayList<Integer> ys;
        int cnt;

        public State(int x) {
            this.x = x;
            this.ys = new ArrayList<>();
            this.cnt = 0;
        }

        double calcAverage() {
            double sum = 0.;
            for (Integer y : ys)
                sum += (double) y;

            return sum / (double) ys.size();
        }

        BigDecimal calcVariance() {
            BigDecimal av = BigDecimal.valueOf(calcAverage());
            BigDecimal sum = BigDecimal.ZERO;
            BigDecimal tmp;
            for (Integer y : ys) {
                tmp = BigDecimal.valueOf(y).subtract(av);
                sum = sum.add(tmp.multiply(tmp));
            }
            tmp = BigDecimal.valueOf(ys.size());
            return sum.divide(tmp, BigDecimal.ROUND_HALF_UP).setScale(16, BigDecimal.ROUND_HALF_UP);
        }

    }

    public void run() throws IOException {
        Locale.setDefault(Locale.ENGLISH);

        try (
                Reader r = new InputStreamReader(System.in);
                BufferedReader br = new BufferedReader(r);
        ) {
            int K = Integer.parseInt(br.readLine());
            int N = Integer.parseInt(br.readLine());

            State[] states = new State[K];
            for (int i = 0; i < K; i++) {
                states[i] = new State(i);
            }

            for (int i = 0; i < N; i++) {
                StringTokenizer st = new StringTokenizer(br.readLine());
                int x = Integer.parseInt(st.nextToken()),
                        y = Integer.parseInt(st.nextToken());
                states[x - 1].ys.add(y);
                states[x - 1].cnt++;
            }

            BigDecimal answer = BigDecimal.ZERO;
            for (State s : states) {
                if (s.cnt == 0) continue;
                answer = answer.add(BigDecimal.valueOf((double) s.cnt / (double) N).multiply(s.calcVariance()))
                        .setScale(16, BigDecimal.ROUND_HALF_UP);
            }

            System.out.println(String.format("%.16f", answer));
        }
    }

    public static void main(String[] args) throws IOException {
        Locale.setDefault(Locale.US);
        new condVariance().run();
    }

}