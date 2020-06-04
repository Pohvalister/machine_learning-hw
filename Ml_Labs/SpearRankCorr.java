import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.Reader;
import java.math.BigDecimal;

import java.util.*;

public class SpearRankCorr {

    public static void main(String... args) throws IOException {
        Locale.setDefault(Locale.ENGLISH);

        try (
                Reader r = new InputStreamReader(System.in);
                BufferedReader br = new BufferedReader(r);
        ) {

            int N = Integer.parseInt(br.readLine());

            List<Pair> x1Feat = new ArrayList<>(),
                    x2Feat = new ArrayList<>();
            for (int i = 0; i < N; i++) {
                StringTokenizer st = new StringTokenizer(br.readLine());
                int x1 = Integer.parseInt(st.nextToken()),
                        x2 = Integer.parseInt(st.nextToken());
                x1Feat.add(new Pair(x1, i));
                x2Feat.add(new Pair(x2, i));
            }

            Comparator <Pair> pairComp
                    = (a, b) -> a.F == b.F
                    ? Integer.compare (a.S, b.S)
                    : Integer.compare (a.F, b.F);

            x1Feat.sort(pairComp);
            x2Feat.sort(pairComp);

            int[] x1R = new int[N];
            int[] x2R = new int[N];
            for (int i = 0; i < N; i++) {
                x1R[x1Feat.get(i).S] = x2R[x2Feat.get(i).S] = i;
            }

            BigDecimal sum = new BigDecimal("0");
            for (int i =0; i< N; i++) {
                BigDecimal d = BigDecimal.valueOf(x1R[i]).subtract(BigDecimal.valueOf(x2R[i]));
                sum = sum.add(d.multiply(d));
            }
            sum = sum.setScale(10, BigDecimal.ROUND_HALF_UP);
            BigDecimal nD = BigDecimal.valueOf(N);
            BigDecimal n2DMinusOne = nD.multiply(nD).subtract(BigDecimal.ONE);
            BigDecimal divider = nD.multiply(n2DMinusOne).setScale(10, BigDecimal.ROUND_HALF_UP);
            BigDecimal s = sum.divide(divider, BigDecimal.ROUND_HALF_UP).setScale(10, BigDecimal.ROUND_HALF_UP);
            s = s.multiply(new BigDecimal("6"));
            s = BigDecimal.ONE.subtract(s);
            out.println(s);
        }
    }

    public static class Pair {

        public final int F, S;

        public Pair(int f, int s) {
            this.F = f;
            this.S = s;
        }

    }

}