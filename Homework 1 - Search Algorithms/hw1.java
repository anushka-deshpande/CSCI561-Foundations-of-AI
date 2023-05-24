import java.io.*;
import java.util.*;

public class homework
{

    static class Pair
    {
        private final int x, y;
        private int cost;
        private int M;

        private int g_score;
        private int h_score;
        private int f_score;
        public Pair(int x, int y, int c)
        {
            this.x = x;
            this.y = y;
            this.cost = c;

        }

        @Override
        public int hashCode() {
            return Objects.hash(x, y);
        }

        @Override
        public boolean equals(Object o)
        {
            if (this == o)
                return true;
            if (o == null)
                return false;
            if (getClass() != o.getClass())
                return false;

            Pair other = (Pair) o;
            if (x != other.x)
                return false;
            if (y != other.y)
                return false;
            return true;
        }
    }

    private String algorithm;
    private int W, H, X, Y, S, N;

    private final HashMap<Pair,Pair> lodges = new HashMap<>();
    private final ArrayList<Pair> lodgeOrder = new ArrayList<>();
    private final ArrayList<ArrayList<Integer>> matrix = new ArrayList<>();

    public static void main(String [] args) throws IOException
    {
        homework obj = new homework();
        obj.getInput();

        /*System.out.println("Algo: " + obj.algorithm);
        System.out.println("\nW: " + obj.W);
        System.out.println("H: " + obj.H);
        System.out.println("\nStart: " + obj.X + " " + obj.Y);
        System.out.println("\nStamina: " + obj.S);
        System.out.println("\nLodges: " + obj.N);
        System.out.println("\nLocations: \n" + obj.lodges);
        System.out.println("\nMatrix: \n" + obj.matrix);*/

        switch (obj.algorithm)
        {
            case "BFS" -> obj.BFS();
            case "UCS" -> obj.UCS();
            case "A*" -> obj.AStar();
        }
    }

    public void getInput() throws FileNotFoundException
    {
        File file = new File("input.txt");
        Scanner sc = new Scanner(file);

        algorithm = sc.next();
        W = sc.nextInt();
        H = sc.nextInt();

        X = sc.nextInt();
        Y = sc.nextInt();

        S = sc.nextInt();
        N = sc.nextInt();

        for(int i=0;i<N;i++)
        {
            int y = sc.nextInt();
            int x = sc.nextInt();
            Pair lodge = new Pair(x, y, 0);
            Pair t = null;
            lodges.put(lodge, t);
            lodgeOrder.add(lodge);
        }

        for(int i=0;i<H;i++)
        {
            ArrayList<Integer> row = new ArrayList<Integer>();
            for(int j=0;j<W;j++)
            {
                row.add(sc.nextInt());
            }
            matrix.add(row);
        }
    }

    public boolean isValid(Pair location)
    {
        return location.x >= 0 && location.y >= 0 && location.x < H && location.y < W;
    }

    public boolean validMove(Pair curr, Pair next)
    {
        if(matrix.get(next.x).get(next.y) < 0) //if tree
        {
            //if tree is taller
            return Math.abs(matrix.get(next.x).get(next.y)) <= Math.abs(matrix.get(curr.x).get(curr.y));

        }
        else if(matrix.get(next.x).get(next.y) < Math.abs(matrix.get(curr.x).get(curr.y))) // if next ht is less than curr ht
        {
            return true;
        }
        else if(matrix.get(next.x).get(next.y) - Math.abs(matrix.get(curr.x).get(curr.y)) <= S) // if next is higher than curr
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    public void BFS() throws IOException
    {
        int[][] adj = {{-1, 0}, {1,0}, {0, 1}, {0, -1}, {1, 1}, {-1, 1}, {1, -1}, {-1, -1}};

        Queue<Pair> q = new LinkedList<>();
        HashMap<Pair, Integer> visited = new HashMap<>();
        HashMap<Pair, Pair> parent = new HashMap<>();

        Pair start = new Pair(Y, X, 0);
        q.add(start);
        visited.put(start,0);

        while(!q.isEmpty())
        {
            Pair curr = q.peek();

            for(int i=0;i<8;i++)
            {
                Pair next = new Pair(curr.x + adj[i][0], curr.y + adj[i][1], curr.cost + 1);

                if(isValid(next))
                {
                    if (validMove(curr, next))
                    {
                        if(!visited.containsKey(next))
                        {
                            //System.out.println(next.x + ", " + next.y);
                            visited.put(next, curr.cost + 1);
                            parent.put(next, curr);
                            q.add(next);
                        }
                    }
                }
            }
            q.remove();
        }

        //backtracking for path
        BufferedWriter writer = new BufferedWriter(new FileWriter("output.txt", false));
        int counter = 0;
        for(Pair lodge: lodgeOrder)
        {

            if(parent.containsKey(lodge))
            {
                ArrayList<Pair> back = new ArrayList<>();
                //back.add(lodge);

                Pair prev;
                Pair curr = lodge;

                while(!Objects.equals(curr, start))
                {
                    back.add(curr);
                    prev = curr;
                    curr = parent.get(prev);
                }
                back.add(start);
                //System.out.println("Path: ");

                for(int i = back.size()-1; i >= 0; i--)
                {
                    //System.out.println(back.get(i).x + ", " + back.get(i).y);
                    if(i == 0)
                        writer.write(back.get(i).y + "," + back.get(i).x);
                    else
                        writer.write(back.get(i).y + "," + back.get(i).x + " ");
                }
            }
            else
            {
                //System.out.println("NULL");
                writer.write("FAIL");
            }
            counter++;
            if(counter != N)
                writer.newLine();
        }
        writer.close();

    }

    public void UCS() throws IOException {

        int[][] adj = {
                {-1, 0, 10},
                {0, 1, 10},
                {1, 0, 10},
                {0, -1, 10},
                {-1, 1, 14},
                {1, 1, 14},
                {1, -1, 14},
                {-1, -1, 14}};

        PriorityQueue<Pair> queue = new PriorityQueue<>(new Comparator<Pair>()
        {
            @Override
            public int compare(Pair o1, Pair o2)
            {
                return Integer.compare(o1.cost, o2.cost);
            }
        });

        queue.add(new Pair(Y, X, 0));

        ArrayList<Pair> visited = new ArrayList<>();
        HashMap<Pair, Integer> distance = new HashMap<>();
        HashMap<Pair, Pair> parent = new HashMap<>();

        distance.put(new Pair(Y, X, 0), 0);
        parent.put(new Pair(Y, X, 0), new Pair(-1, -1, 0));

        while(!queue.isEmpty())
        {
            Pair curr = queue.poll();
            visited.add(curr);

            for(int i=0;i<8;i++)
            {
                Pair next = new Pair(curr.x + adj[i][0], curr.y + adj[i][1], curr.cost + adj[i][2]);

                if(isValid(next) && validMove(curr, next) && !visited.contains(next))
                {
                    if((distance.get(next) == null) || (distance.get(curr) + adj[i][2] < distance.get(next)))
                    {
                        if(distance.get(next) == null)
                            distance.put(next, distance.get(curr) + adj[i][2]);
                        else
                            distance.replace(next, distance.get(curr) + adj[i][2]);

                        parent.put(next, curr);
                        queue.add(next);

                        if(lodges.containsKey(next) && (lodges.get(next) == null || lodges.get(next).cost > (distance.get(curr) + adj[i][2])))
                        {
                            lodges.replace(next, curr);
                        }

                    }
                }
            }
        }

        //backtracking for path
        BufferedWriter writer = new BufferedWriter(new FileWriter("output.txt", false));
        int counter = 0;
        for(Pair lodge: lodgeOrder)
        {
            if(!Objects.equals(lodges.get(lodge), null))
            {
                ArrayList<Pair> back = new ArrayList<>();
                back.add(lodge);

                Pair prev;
                Pair cur = lodges.get(lodge);

                while(!Objects.equals(cur, new Pair(-1, -1, 0)))
                {
                    back.add(cur);
                    prev = cur;
                    cur = parent.get(prev);
                }

                //System.out.println("Path: ");
                for(int i = back.size()-1; i >= 0; i--)
                {
                    //System.out.print(back.get(i).x + ", " + back.get(i).y);
                    if(i == 0)
                        writer.write(back.get(i).y + "," + back.get(i).x);
                    else
                        writer.write(back.get(i).y + "," + back.get(i).x + " ");
                }
                //System.out.println();
            }
            else
            {
                //System.out.println("NULL");
                writer.write("FAIL");
            }
            counter++;
            if(counter != N)
                writer.newLine();
        }
        writer.close();
    }

    public boolean validMoveAStar(Pair curr, Pair next)
    {
        if(matrix.get(next.x).get(next.y) < 0) //if tree
        {
            //if tree is taller
            return Math.abs(matrix.get(next.x).get(next.y)) <= Math.abs(matrix.get(curr.x).get(curr.y));

        }
        else
        {
            //int M = 0;
            if (matrix.get(next.x).get(next.y) > Math.abs(matrix.get(curr.x).get(curr.y))) // if next ht is less than curr ht
            {
                return matrix.get(next.x).get(next.y) <= (Math.abs(matrix.get(curr.x).get(curr.y)) + S + curr.M);
            }
            else if(matrix.get(next.x).get(next.y) <= Math.abs(matrix.get(curr.x).get(curr.y)))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }

    public int heuristic(Pair curr, Pair dest)
    {
        int del_x = Math.abs(curr.x - dest.x);
        int del_y = Math.abs(curr.y - dest.y);
        int h = 10 * (del_x + del_y) + (14 - 2 * 10) * Math.min(del_x, del_y);
        return h;
    }

    public int getCost(Pair curr, Pair next, int h_cost)
    {
        int e_cost = 0;
        if(matrix.get(next.x).get(next.y) - matrix.get(curr.x).get(curr.y) > curr.M)
        {
            e_cost = Math.max(0, matrix.get(next.x).get(next.y) - matrix.get(curr.x).get(curr.y) - curr.M);
        }

        return h_cost + e_cost;
    }

    public int getM(Pair curr, Pair prev)
    {
        return Math.max(0, matrix.get(prev.x).get(prev.y) - matrix.get(curr.x).get(curr.y));
    }
    public void AStar() throws IOException {

        BufferedWriter writer = new BufferedWriter(new FileWriter("output.txt", false));
        int counter = 0;

        int[][] adj = {
                {-1, 0, 10},
                {0, 1, 10},
                {1, 0, 10},
                {0, -1, 10},
                {-1, 1, 14},
                {1, 1, 14},
                {1, -1, 14},
                {-1, -1, 14}};

        for(Pair lodge: lodgeOrder)
        {
            HashMap<Pair, Integer> visited = new HashMap<>();
            HashMap<Pair, Pair> parent = new HashMap<>();

            PriorityQueue<Pair> queue = new PriorityQueue<>(new Comparator<Pair>()
            {
                @Override
                public int compare(Pair o1, Pair o2)
                {
                    return Integer.compare(o1.f_score, o2.f_score);
                }
            });

            Pair start = new Pair(Y, X, 0);
            parent.put(start, new Pair(-1, -1, 0));
            start.g_score = 0;
            start.M = 0;
            //System.out.println("Start: " + start.x + " " + start.y);
            //System.out.println("Lodge:  " + lodge.x + " " + lodge.y);
            start.h_score = heuristic(start, lodge);

            queue.add(start);
            boolean found = false;

            while(!queue.isEmpty() && !found)
            {
                //Node having lowest f_score
                Pair curr = queue.poll();
                visited.put(curr, curr.f_score);

                //goal found
                if(curr.equals(lodge))
                {
                    lodges.replace(lodge, parent.get(curr));
                    found = true;
                }

                //check every child of curr node

                for(int i=0;i<8;i++)
                {

                    Pair child = new Pair(curr.x + adj[i][0], curr.y + adj[i][1], 0);

                    if(isValid(child) && validMoveAStar(curr, child))
                    {

                        //System.out.println("Child: " + child.x + " " + child.y);

                        child.M = getM(child, curr);
                        //System.out.println("M: " + child.M);

                        child.cost = getCost(curr, child, adj[i][2]);
                        //System.out.println("Cost: " + child.cost);

                        child.h_score = heuristic(child, lodge);
                        //System.out.println("H_score: " + child.h_score);

                        int cost = child.cost;
                        int temp_g = curr.g_score + cost;
                        int temp_f = temp_g + child.h_score;

                        //if child has been evaluated and newer f_score is higher,skip
                        if (visited.containsKey(child) && temp_f >= child.f_score)
                            continue;

                            // else if child node is not in queue or newer f_score is lower
                        else if (!queue.contains(child) || temp_f < child.f_score)
                        {
                            if (parent.containsKey(child))
                                parent.replace(child, curr);
                            else
                                parent.put(child, curr);

                            child.g_score = temp_g;
                            //System.out.println("G_score: " + child.g_score);

                            child.f_score = temp_f;
                            //System.out.println("F_score: " + child.f_score);
                            //System.out.println();

                            if (queue.contains(child))
                            {
                                queue.remove(child);
                            }

                            queue.add(child);
                        }
                    }
                }

            }

            //System.out.println("complete");
            if(!Objects.equals(lodges.get(lodge), null))
            {
                ArrayList<Pair> back = new ArrayList<>();
                back.add(lodge);

                Pair prev;
                Pair cur = lodges.get(lodge);

                while(!Objects.equals(cur, new Pair(-1, -1, 0)))
                {
                    back.add(cur);
                    prev = cur;
                    cur = parent.get(prev);
                }

                if(visited.containsKey(lodge))
                    //System.out.println("Cost: " + visited.get(lodge));

                    for(int i = back.size()-1; i >= 0; i--)
                    {
                        //System.out.print(back.get(i).x + ", " + back.get(i).y);
                        if(i == 0)
                            writer.write(back.get(i).y + "," + back.get(i).x);
                        else
                            writer.write(back.get(i).y + "," + back.get(i).x + " ");
                    }
                //System.out.println();
            }
            else
            {
                //System.out.println("NULL");
                writer.write("FAIL");
            }
            counter++;
            if(counter != N)
                writer.newLine();
        }
        writer.close();
    }
}
