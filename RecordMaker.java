import java.util.Random;

public class RecordMaker
{
    private static boolean[] flag = new boolean[100000];
    public static void main(String[] args)
    {
        int count = 0;
        if(args.length != 0)
            count = Integer.parseInt(args[0]);
        for(int i=0; i<count; i++)
        {
            Random random = new Random();
            int num;
            while(flag[num = random.nextInt(100000)]);
            flag[num] = true;
            int score = random.nextInt(100);
            System.out.printf("NT%05d %d", num, score);
            System.out.println();
        }
    }
}
