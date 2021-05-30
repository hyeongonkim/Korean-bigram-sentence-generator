package pm10pm25goodstation;

import java.io.IOException;
import java.util.StringTokenizer;
import org.apache.hadoop.io.DoubleWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Mapper;

public class GoodStationMapper extends Mapper<Object, Text, Text, Text> {

  Text ok = new Text();
  Text ov = new Text();

  // 2017-01-01 00:00,101,1,0.004,0
  @Override
  protected void map(Object key, Text value,
      Mapper<Object, Text, Text, Text>.Context context)
      throws IOException, InterruptedException {

    StringTokenizer st = new StringTokenizer(value.toString(), ",");
    st.nextToken();

    String stationCode = st.nextToken();
    String itemCode = st.nextToken();
    double measuredValue = Double.parseDouble(st.nextToken());
    if (itemCode.equals("8")) {
      if (measuredValue <= 30 && measuredValue >= 0) {
        ok.set(itemCode);
        ov.set(stationCode);
        context.write(ok, ov);
      }
    } else if (itemCode.equals("9")) {
      if (measuredValue <= 15 && measuredValue >= 0) {
        ok.set(itemCode);
        ov.set(stationCode);
        context.write(ok, ov);
      }
    }

  }
}
