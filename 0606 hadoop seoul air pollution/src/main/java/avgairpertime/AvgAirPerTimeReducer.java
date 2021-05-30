package avgairpertime;

import java.io.IOException;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Reducer;

public class AvgAirPerTimeReducer extends Reducer<Text, Text, Text, Text> {

  Text ov = new Text();

  @Override
  protected void reduce(Text key, Iterable<Text> values,
      Reducer<Text, Text, Text, Text>.Context context)
      throws IOException, InterruptedException {

    double so2Sum = 0;
    int so2Cnt = 0;
    double no2Sum = 0;
    int no2Cnt = 0;
    double coSum = 0;
    int coCnt = 0;
    double o3Sum = 0;
    int o3Cnt = 0;
    double pm10Sum = 0;
    int pm10Cnt = 0;
    double pm25Sum = 0;
    int pm25Cnt = 0;

    for (Text v : values) {
      String[] iterValue = v.toString().split(":");
      if ("1".equals(iterValue[0])) {
        so2Sum += Double.parseDouble(iterValue[1]);
        so2Cnt++;
      } else if ("3".equals(iterValue[0])) {
        no2Sum += Double.parseDouble(iterValue[1]);
        no2Cnt++;
      } else if ("5".equals(iterValue[0])) {
        coSum += Double.parseDouble(iterValue[1]);
        coCnt++;
      } else if ("6".equals(iterValue[0])) {
        o3Sum += Double.parseDouble(iterValue[1]);
        o3Cnt++;
      } else if ("8".equals(iterValue[0])) {
        pm10Sum += Double.parseDouble(iterValue[1]);
        pm10Cnt++;
      } else if ("9".equals(iterValue[0])) {
        pm25Sum += Double.parseDouble(iterValue[1]);
        pm25Cnt++;
      }
    }

    double so2Avg = so2Sum / so2Cnt;
    double no2Avg = no2Sum / no2Cnt;
    double coAvg = coSum / coCnt;
    double o3Avg = o3Sum / o3Cnt;
    double pm10Avg = pm10Sum / pm10Cnt;
    double pm25Avg = pm25Sum / pm25Cnt;

    ov.set("SO2:" + so2Avg + " / NO2:" + no2Avg + " / CO:" + coAvg + " / O3:" + o3Avg + " / PM10:" + pm10Avg + " / PM2.5:" + pm25Avg);
    context.write(key, ov);
  }
}
