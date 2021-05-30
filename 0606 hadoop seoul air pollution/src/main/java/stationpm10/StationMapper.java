package stationpm10;

import java.io.IOException;
import java.util.StringTokenizer;
import org.apache.hadoop.io.DoubleWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

public class StationMapper extends Mapper<Object, Text, Text, DoubleWritable> {

  Text ok = new Text();
  DoubleWritable ov = new DoubleWritable();

  @Override
  protected void map(Object key, Text value,
      Mapper<Object, Text, Text, DoubleWritable>.Context context)
      throws IOException, InterruptedException {

    StringTokenizer st = new StringTokenizer(value.toString(), ",");
    st.nextToken();

    String stationCode = st.nextToken();
    String itemCode = st.nextToken();
    if (itemCode.equals("8")) {
      ok.set(stationCode + "\t" + itemCode);
      double measuredValue = Double.parseDouble(st.nextToken());
      ov.set(measuredValue);
      context.write(ok, ov);
    }
  }
}
