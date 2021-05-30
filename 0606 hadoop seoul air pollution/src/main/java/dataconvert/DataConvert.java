package dataconvert;

import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;

public class DataConvert extends Configured implements Tool {

  public static void main(String[] args) throws Exception {
    ToolRunner.run(new DataConvert(), args);
  }

  public int run(String[] args) throws Exception {

    String inputPath = args[0];
    String outputPath = args[0] + ".out";

    Job job = Job.getInstance(getConf());
    job.setJarByClass(DataConvert.class);

    job.setMapperClass(DataConvertMapper.class);
    job.setReducerClass(DataConvertReducer.class);

    job.setMapOutputKeyClass(Text.class);
    job.setMapOutputValueClass(Text.class);

    job.setInputFormatClass(TextInputFormat.class);
    job.setOutputFormatClass(TextOutputFormat.class);

    FileInputFormat.addInputPath(job, new Path(inputPath));
    FileOutputFormat.setOutputPath(job, new Path(outputPath));

    job.waitForCompletion(true);

    return 0;
  }
}
