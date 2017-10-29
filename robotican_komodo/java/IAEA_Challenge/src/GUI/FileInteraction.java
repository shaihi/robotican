package GUI;

import java.io.*;
import java.util.Scanner;


public class FileInteraction {
	

	public static double[] getScale(String yaml) {

        // This will reference one line at a time
        String line = null;

        try {
            // FileReader reads text files in the default encoding.
        	java.io.FileReader fileReader =  new java.io.FileReader(yaml);

            // Always wrap FileReader in BufferedReader.
            BufferedReader bufferedReader =  new BufferedReader(fileReader);

//            while((line = bufferedReader.readLine()) != null) {
//                System.out.println(line);
//            }      
            line = bufferedReader.readLine();
            line = bufferedReader.readLine();
            String[] s1 = line.split(" ");
            double resolution=Double.parseDouble(s1[1]);

            line = bufferedReader.readLine();
            line = line.substring(9, line.length()-1);
            String[] s = line.split(", ");
            double x=Double.parseDouble(s[0]);
            double y=Double.parseDouble(s[1]);

            // Always close files.
            bufferedReader.close(); 
            return new double[] {resolution,x,y};
        }
        catch(FileNotFoundException ex) {
            System.out.println(
                "Unable to open file '" + 
                		yaml + "'"); 
            return new double[] {-1,-1,-1};
        }
        catch(IOException ex) {
            System.out.println(
                "Error reading file '" 
                + yaml + "'");
            return new double[] {-1,-1,-1};
        }
    }
	
	public static void write(String fileName ,String output) {
		try (Writer writer = new BufferedWriter(new OutputStreamWriter(
	              new FileOutputStream(fileName), "utf-8"))) {
	   writer.write(output);
		} 
		catch (IOException e) {
			System.out.println("ERROR: file not created");
			e.printStackTrace();
		}
	}
	public static void updateSaveMap(String mapName) {
		String output="#!/bin/bash\n" + 
				"\n" + 
				"\n" + 
				"export ROSLAUNCH_SSH_UNKNOWN=1\n" + 
				"export ROS_MASTER_URI=http://192.168.0.100:11311\n" + 
				"export ROS_IP=192.168.0.101\n" + 
				"\n" + 
				"source /opt/ros/indigo/setup.bash\n" + 
				"source ~/catkin_ws/devel/setup.bash\n" + 
				"\n" + 
				"exec \"$@\"\n" + 
				"cd /home/or/catkin_ws/src/komodo-iaea-challenge/robotican_komodo/java/IAEA_Challenge/maps/currmap\n" + 
				"rosrun map_server map_saver -f "+mapName+"\n" + 
				"\n" + 
				"sshpass -p 'or2591989' scp -r /home/or/catkin_ws/src/komodo-iaea-challenge/robotican_komodo/java/IAEA_Challenge/maps/currmap avishys_lab_ubuntu@192.168.0.100:/home/avishys_lab_ubuntu/catkin_ws/src/komodo-iaea-challenge/robotican_komodo/maps/ \n" + 
				"\n" + 
				"convert "+mapName+".pgm "+mapName+".png\n" + 
				"\n" + 
				"";
		
		try (Writer writer = new BufferedWriter(new OutputStreamWriter(
	              new FileOutputStream("/home/or/catkin_ws/src/komodo-iaea-challenge/robotican_komodo/java/IAEA_Challenge/ros/save_map.sh"), "utf-8"))) {
	   writer.write(output);
		} 
		catch (IOException e) {
			System.out.println("ERROR: file not created");
			e.printStackTrace();
		}
	}

	public static void updateMap(String mapName) {
		String output ="<?xml version=\"1.0\"?>\n" + 
				"<launch>\n" + 
				"  \n" + 
				"  <machine name=\"avishys_lab_ubuntu\" address=\"192.168.0.100\" user=\"avishys_lab_ubuntu\" password=\"or2591989\" env-loader=\"/home/or/komodo_rc.sh\" default=\"true\"/>\n" + 
				"  \n" + 
				"  <arg name=\"scan\" default=\"scan\" />\n" + 
				"  <arg name=\"x\" default=\"0.0\"/>\n" + 
				"  <arg name=\"y\" default=\"0.0\"/>\n" + 
				"  <arg name=\"Y\" default=\"0.0\" />\n" + 
				"  <arg name=\"map_file\" default=\"$(find robotican_komodo)/maps/"+mapName+".yaml\"/>\n" + 
				"  \n" + 
				"  \n" + 
				"  <node machine=\"avishys_lab_ubuntu\" name=\"map_server\" pkg=\"map_server\" type=\"map_server\" args=\"$(arg map_file)\" />\n" + 
				"  \n" + 
				"  <node machine=\"avishys_lab_ubuntu\" pkg=\"amcl\" type=\"amcl\" name=\"amcl\">\n" + 
				"	<remap from=\"scan\" to=\"$(arg scan)\"/> \n" + 
				"    <param name=\"save_pose_rate\" value=\"1.0\"/>\n" + 
				"    <param name=\"initial_cov_xx\" value=\"0.1\"/>\n" + 
				"    <param name=\"initial_cov_yy\" value=\"0.1\"/>\n" + 
				"    <param name=\"initial_cov_aa\" value=\"0.1\"/>\n" + 
				"    <param name=\"use_map_topic\" value=\"true\"/>\n" + 
				"    <param name=\"odom_model_type\" value=\"diff-corrected\"/>\n" + 
				"    <param name=\"odom_alpha1\" value=\"0.03\"/>\n" + 
				"    <param name=\"odom_alpha2\" value=\"0.03\"/>\n" + 
				"    <param name=\"odom_alpha3\" value=\"0.03\"/>\n" + 
				"    <param name=\"odom_alpha4\" value=\"0.03\"/>\n" + 
				"    <param name=\"gui_publish_rate\" value=\"1.0\"/>\n" + 
				"    <param name=\"laser_max_beams\" value=\"60\"/>\n" + 
				"    <param name=\"laser_max_range\" value=\"30\"/>\n" + 
				"    <param name=\"laser_min_range\" value=\"0.02\"/>\n" + 
				"    <param name=\"min_particles\" value=\"30\"/>\n" + 
				"    <param name=\"max_particles\" value=\"6000\"/>\n" + 
				"    <param name=\"kld_err\" value=\"0.05\"/>\n" + 
				"    <param name=\"kld_z\" value=\"0.993\"/>\n" + 
				"    <param name=\"odom_alpha1\" value=\"0.02\"/>\n" + 
				"    <param name=\"odom_alpha2\" value=\"0.02\"/>\n" + 
				"    <param name=\"odom_alpha3\" value=\"0.02\"/>\n" + 
				"    <param name=\"odom_alpha4\" value=\"0.02\"/>\n" + 
				"    <param name=\"laser_z_hit\" value=\"0.5\"/>\n" + 
				"    <param name=\"laser_z_short\" value=\"0.05\"/>\n" + 
				"    <param name=\"laser_z_max\" value=\"0.05\"/>\n" + 
				"    <param name=\"laser_z_rand\" value=\"0.5\"/>\n" + 
				"    <param name=\"laser_sigma_hit\" value=\"0.2\"/>\n" + 
				"    <param name=\"laser_lambda_short\" value=\"0.1\"/>\n" + 
				"    <param name=\"laser_model_type\" value=\"likelihood_field_prob\"/>\n" + 
				"    <param name=\"laser_likelihood_max_dist\" value=\"2.0\"/>\n" + 
				"    <param name=\"update_min_d\" value=\"0.2\"/>\n" + 
				"    <param name=\"update_min_a\" value=\"0.5\"/>\n" + 
				"    <param name=\"odom_frame_id\" value=\"odom\"/>\n" + 
				"    <param name=\"resample_interval\" value=\"2\"/>\n" + 
				"    <param name=\"transform_tolerance\" value=\"0.05\"/>\n" + 
				"    <param name=\"recovery_alpha_slow\" value=\"0.002\"/>\n" + 
				"    <param name=\"recovery_alpha_fast\" value=\"0.15\"/>   \n" + 
				"    <param name=\"initial_pose_x\" value=\"$(arg x)\"/>  \n" + 
				"    <param name=\"initial_pose_y\" value=\"$(arg y)\"/>\n" + 
				"    <param name=\"initial_pose_a\" value=\"$(arg Y)\"/>\n" + 
				"    \n" + 
				"    <param name=\"base_frame_id\" value=\"base_link\"/>\n" + 
				"    <param name=\"tf_broadcast\" value=\"true\"/>\n" + 
				"  </node>\n" + 
				"\n" + 
				"</launch>";		
		
		try (Writer writer = new BufferedWriter(new OutputStreamWriter(
	              new FileOutputStream("/home/or/catkin_ws/src/komodo-iaea-challenge/robotican_komodo/launch/remote/amcl_remote.launch"), "utf-8"))) {
	   writer.write(output);
		} 
		catch (IOException e) {
			System.out.println("ERROR: file not created");
			e.printStackTrace();
		}
	}

}
