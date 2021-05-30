package dataconvert;

import java.io.IOException;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

public class DataConvertReducer extends Reducer<Text, Text, Text, Text> {

  Text ov = new Text();

  @Override
  protected void reduce(Text key, Iterable<Text> values,
      Reducer<Text, Text, Text, Text>.Context context)
      throws IOException, InterruptedException {

    StringBuilder newValue = new StringBuilder();
    for (Text v : values) {
      String iterValue = v.toString();
      newValue.append("\t").append(iterValue);
    }

    ov.set(newValue.toString());
    context.write(key, ov);
  }
}
