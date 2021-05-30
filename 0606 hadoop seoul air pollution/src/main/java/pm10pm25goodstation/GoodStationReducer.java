package pm10pm25goodstation;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

public class GoodStationReducer extends Reducer<Text, Text, Text, Text> {

  Text ov = new Text();

  @Override
  protected void reduce(Text key, Iterable<Text> values,
      Reducer<Text, Text, Text, Text>.Context context)
      throws IOException, InterruptedException {

    final Counter<String> counts = new Counter<>();

    for (Text v : values) {
      counts.add(v.toString());
    }

    ov.set(counts.findMax());
    context.write(key, ov);
  }
}

class Counter<T> {
  final Map<T, Integer> counts = new HashMap<>();

  public T findMax() {
    Entry<T, Integer> max = null;
    for (Entry<T, Integer> e : counts.entrySet()) {
      if (max == null || e.getValue() > max.getValue())
        max = e;
    }
    assert max != null;
    return max.getKey();
  }

  public void add(T t) {
    counts.merge(t, 1, Integer::sum);
  }

  public int count(T t) {
    return counts.getOrDefault(t, 0);
  }
}