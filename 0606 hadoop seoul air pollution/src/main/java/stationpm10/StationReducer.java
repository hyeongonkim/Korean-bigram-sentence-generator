package stationpm10;

import java.io.IOException;
import org.apache.hadoop.io.DoubleWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

public class StationReducer extends Reducer<Text, DoubleWritable, Text, Text> {

  Text ov = new Text();

  @Override
  protected void reduce(Text key, Iterable<DoubleWritable> values,
      Reducer<Text, DoubleWritable, Text, Text>.Context context)
      throws IOException, InterruptedException {

    double min = Double.POSITIVE_INFINITY;
    double max = Double.NEGATIVE_INFINITY;
    double sum = 0.0;
    int cnt = 0;

    for (DoubleWritable v : values) {
      double iterValue = v.get();
      if (iterValue < 0) continue;

      if (iterValue < min) min = iterValue;
      if (max < iterValue) max = iterValue;

      sum += iterValue;
      cnt++;
    }

    double avg = sum / cnt;

    ov.set(avg + "\t" + max + "\t" + min);
    context.write(key, ov);
  }
}
