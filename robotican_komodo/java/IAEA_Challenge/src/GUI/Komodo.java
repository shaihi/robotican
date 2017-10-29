package GUI;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.EventQueue;
import javax.swing.JFrame;
import java.awt.Image;
import java.awt.GridBagLayout;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import java.awt.GridBagConstraints;
import java.awt.Insets;
import java.awt.Toolkit;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.WindowEvent;
import java.io.File;
import java.io.IOException;
import java.awt.Font;
import java.awt.GraphicsDevice;
import java.awt.GraphicsEnvironment;

public class Komodo {

	private JFrame frame;
	private String MAP = "currMap";

	/**
	 * Launch the application.
	 */
	public static void main(String[] args) {
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				try {
					Komodo window = new Komodo();
					window.frame.setVisible(true);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});
	}

	/**
	 * Create the application.
	 */
	public Komodo() {
		initialize();
	}

	/**
	 * Initialize the contents of the frame.
	 */
	private void initialize() {
		frame = new JFrame();
		frame.setExtendedState(frame.getExtendedState() | JFrame.MAXIMIZED_BOTH); //make frame full screen
		//frame.setUndecorated(true);
		frame.setVisible(true);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		GridBagLayout gridBagLayout = new GridBagLayout();
		
		Dimension screenSize = getSmallestScreen(); //get screen size for GUI
		final double SCREEN_WIDTH = screenSize.getWidth();
		final double SCREEN_HEIGHT = screenSize.getHeight();
		int width = (int) ((SCREEN_WIDTH-530));
		int height = (int) (SCREEN_HEIGHT-140)/5;
		
		gridBagLayout.columnWidths = new int[]{0, 0, 0, 0, 0, 0, width, 0, 0, 0, 0, 0, 0};
		gridBagLayout.rowHeights = new int[]{0, height, height, height, height, height, 0, 0};
		gridBagLayout.columnWeights = new double[]{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Double.MIN_VALUE};
		gridBagLayout.rowWeights = new double[]{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Double.MIN_VALUE};
		frame.getContentPane().setLayout(gridBagLayout);
		
		Image img = new ImageIcon(this.getClass().getResource("/logo2.png")).getImage();
		JLabel lblRobotican = new JLabel("");
		lblRobotican.setIcon(new ImageIcon(img)); //add robotican logo	
		
		GridBagConstraints gbc_lblRobotican = new GridBagConstraints();
		gbc_lblRobotican.fill = GridBagConstraints.BOTH;
		gbc_lblRobotican.insets = new Insets(0, 0, 5, 5);
		gbc_lblRobotican.gridx = 5;
		gbc_lblRobotican.gridy = 1;
		frame.getContentPane().add(lblRobotican, gbc_lblRobotican);
		
		
		JButton btnLoadMap = new JButton("Remote Mapping");
		JButton btnLoadMap1 = new JButton("Reset");
		
		btnLoadMap.setFont(new Font("DejaVu Sans", Font.PLAIN, width/20));
		btnLoadMap1.setFont(new Font("DejaVu Sans", Font.PLAIN, width/20));
		JButton btnAbort = new JButton("ABORT");
		JButton btnExit = new JButton("EXIT");
		btnExit.setFont(new Font("DejaVu Sans", Font.PLAIN, width/20));
		JButton btnsaveMap = new JButton("Save");
		btnsaveMap.setFont(new Font("DejaVu Sans", Font.PLAIN, width/20));
		JButton btnendMap = new JButton("End");
		
		btnendMap.setFont(new Font("DejaVu Sans", Font.PLAIN, width/20));
		
		JLabel label = new JLabel("");
		img = new ImageIcon(this.getClass().getResource("/logo1.png")).getImage();
		label.setIcon(new ImageIcon(img));
		GridBagConstraints gbc_label = new GridBagConstraints();
		gbc_label.fill = GridBagConstraints.VERTICAL;
		gbc_label.insets = new Insets(0, 0, 5, 5);
		gbc_label.gridx = 6;
		gbc_label.gridy = 1;
		frame.getContentPane().add(label, gbc_label);
		img = new ImageIcon(this.getClass().getResource("/logo4.png")).getImage();
		
		JLabel label_1 = new JLabel("");
		label_1.setIcon(new ImageIcon(img));
		GridBagConstraints gbc_label_1 = new GridBagConstraints();
		gbc_label_1.insets = new Insets(0, 0, 5, 5);
		gbc_label_1.gridx = 7;
		gbc_label_1.gridy = 1;
		frame.getContentPane().add(label_1, gbc_label_1);
		btnAbort.setForeground(Color.RED);
		btnAbort.setFont(new Font("DejaVu Sans", Font.BOLD, width/20));
		GridBagConstraints gbc_btnAbort = new GridBagConstraints();
		gbc_btnAbort.fill = GridBagConstraints.BOTH;
		gbc_btnAbort.insets = new Insets(0, 0, 5, 5);
		gbc_btnAbort.gridx = 5;
		gbc_btnAbort.gridy = 3;
		frame.getContentPane().add(btnAbort, gbc_btnAbort);

		GridBagConstraints gbc_btnSaveMap = new GridBagConstraints();
		gbc_btnSaveMap.fill = GridBagConstraints.BOTH;
		gbc_btnSaveMap.insets = new Insets(0, 0, 5, 5);
		gbc_btnSaveMap.gridx = 5;
		gbc_btnSaveMap.gridy = 2;
		frame.getContentPane().add(btnsaveMap, gbc_btnSaveMap);
		GridBagConstraints gbc_btnLoadMap = new GridBagConstraints();
		gbc_btnLoadMap.fill = GridBagConstraints.BOTH;
		gbc_btnLoadMap.insets = new Insets(0, 0, 5, 5);
		gbc_btnLoadMap.gridx = 6;
		gbc_btnLoadMap.gridy = 2;
		frame.getContentPane().add(btnLoadMap, gbc_btnLoadMap);
		GridBagConstraints gbc_btnLoadMap1 = new GridBagConstraints();
		gbc_btnLoadMap1.fill = GridBagConstraints.BOTH;
		gbc_btnLoadMap1.insets = new Insets(0, 0, 5, 5);
		gbc_btnLoadMap1.gridx = 7;
		gbc_btnLoadMap1.gridy = 2;
		frame.getContentPane().add(btnLoadMap1, gbc_btnLoadMap1);
		GridBagConstraints gbc_btendMap = new GridBagConstraints();
		gbc_btendMap.fill = GridBagConstraints.BOTH;
		gbc_btendMap.insets = new Insets(0, 0, 5, 5);
		gbc_btendMap.gridx = 7;
		gbc_btendMap.gridy = 3;
		frame.getContentPane().add(btnendMap, gbc_btendMap);
		GridBagConstraints gbc_btExit = new GridBagConstraints();
		gbc_btExit.fill = GridBagConstraints.BOTH;
		gbc_btExit.insets = new Insets(0, 0, 5, 5);
		gbc_btExit.gridx = 7;
		gbc_btExit.gridy = 4;
		frame.getContentPane().add(btnExit, gbc_btExit);
		
		JButton btnCancelAll = new JButton("Load map");
		btnCancelAll.setFont(new Font("DejaVu Sans", Font.PLAIN, width/20));
		
		GridBagConstraints gbc_btnCancelAll = new GridBagConstraints();
		gbc_btnCancelAll.fill = GridBagConstraints.BOTH;
		gbc_btnCancelAll.insets = new Insets(0, 0, 5, 5);
		gbc_btnCancelAll.gridx = 6;
		gbc_btnCancelAll.gridy = 3;
		frame.getContentPane().add(btnCancelAll, gbc_btnCancelAll);
		
		JButton btnNewButton = new JButton(" Plan Mission ");
		btnNewButton.setFont(new Font("DejaVu Sans", Font.PLAIN, width/20));
		GridBagConstraints gbc_btnNewButton = new GridBagConstraints();
		gbc_btnNewButton.fill = GridBagConstraints.BOTH;
		gbc_btnNewButton.insets = new Insets(0, 0, 5, 5);
		gbc_btnNewButton.gridx = 6;
		gbc_btnNewButton.gridy = 4;
		frame.getContentPane().add(btnNewButton, gbc_btnNewButton);
		
		JButton btnStartCruise = new JButton("Start Mission");
		btnStartCruise.setFont(new Font("DejaVu Sans", Font.PLAIN, width/20));
		GridBagConstraints gbc_btnStartCruise = new GridBagConstraints();
		gbc_btnStartCruise.fill = GridBagConstraints.BOTH;
		gbc_btnStartCruise.insets = new Insets(0, 0, 5, 5);
		gbc_btnStartCruise.gridx = 6;
		gbc_btnStartCruise.gridy = 5;
		frame.getContentPane().add(btnStartCruise, gbc_btnStartCruise);
		
 		btnNewButton.setEnabled(false); //disable buttons
		btnStartCruise.setEnabled(false);
		btnLoadMap1.setEnabled(false);
		btnsaveMap.setEnabled(false);
		btnendMap.setEnabled(false);
		
		Runtime rt = Runtime.getRuntime();
		try {
	        Process pr = rt.exec("ros/joystick.sh");
		} catch (IOException e1) {
			System.out.println("ERROR: fail to launch shell file");
			e1.printStackTrace();
		}
		
		btnLoadMap.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseClicked(MouseEvent arg0) { //run rviz and gazebo
				if(btnLoadMap.isEnabled()){
				Runtime rt = Runtime.getRuntime();
				try {
			        Process pr = rt.exec("ros/mapping.sh");
				} catch (IOException e1) {
					System.out.println("ERROR: fail to launch shell file");
					e1.printStackTrace();
				}
				try {
					Thread.sleep(3000);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				try {
			        Process pr2 = rt.exec("ros/mapping1.sh");
				} catch (IOException e1) {
					System.out.println("ERROR: fail to launch shell file");
					e1.printStackTrace();
				}
				btnLoadMap1.setEnabled(true);
				btnsaveMap.setEnabled(true);
				btnendMap.setEnabled(true);
				btnLoadMap.setEnabled(false);
				btnCancelAll.setEnabled(true);
				btnNewButton.setEnabled(true);
				}
				
			}
		});
		btnLoadMap1.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseClicked(MouseEvent arg0) {
				if(btnLoadMap1.isEnabled()){
					Runtime rt = Runtime.getRuntime();
					try {
				        Process pr = rt.exec("ros/reset_map.sh");
					} catch (IOException e1) {
						System.out.println("ERROR: fail to launch shell file");
						e1.printStackTrace();
					}
					try {
				        Process pr = rt.exec("ros/stop.sh");
					} catch (IOException e1) {
						System.out.println("ERROR: fail to launch shell file");
						e1.printStackTrace();
					}
			}
			}
		});
		
		btnAbort.addMouseListener(new MouseAdapter() { //kill processes
			@Override
			public void mouseClicked(MouseEvent e) {
				int abort = JOptionPane.showConfirmDialog(btnLoadMap, "Are you sure?", "Abort and return to origin", 0, 3);
				
				if(abort==0){ // terminate all processes
					try {
				        Runtime rt = Runtime.getRuntime();  
				        Process pr = rt.exec("ros/kill_processes.sh");
					} catch (IOException e1) {
						System.out.println("ERROR: fail to launch shell file");
						e1.printStackTrace();
					}

				}
			}
		});
		
		btnExit.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseClicked(MouseEvent e) {
				int abort = JOptionPane.showConfirmDialog(btnLoadMap, "Are you sure?", "EXIT", 0, 3);
				
				if(abort==0){ // terminate all processes
					try {
				        Runtime rt = Runtime.getRuntime();
				        Process pr = rt.exec("ros/kill_joystick.sh");
				        frame.dispatchEvent(new WindowEvent(frame, WindowEvent.WINDOW_CLOSING));
					} catch (IOException e1) {
						System.out.println("ERROR: fail to launch shell file");
						e1.printStackTrace();
					}

				}
			}
		});
		btnsaveMap.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseClicked(MouseEvent arg0) { //save current map
				if(btnsaveMap.isEnabled()){
					String map = JOptionPane.showInputDialog(frame, "Choose name for map:");
					boolean found=false;
					while(!found && map!=null && !map.equals("")){
						File f = new File("/home/or/catkin_ws/src/komodo-iaea-challenge/robotican_komodo/java/IAEA_Challenge/maps/currmap/"+map+".png");
						if(!f.exists() && !f.isDirectory()) {
							try {
								FileInteraction.updateSaveMap(map); // change map name in shell script
						        Runtime rt = Runtime.getRuntime();
						        Process pr = rt.exec("ros/save_map.sh");
							} catch (IOException e1) {
								System.out.println("ERROR: fail to launch shell file");
								e1.printStackTrace();
							}
						    found=true;
						    btnCancelAll.setEnabled(false);
							btnNewButton.setEnabled(false);
							btnLoadMap.setEnabled(false);
							btnLoadMap1.setEnabled(false);
							btnsaveMap.setEnabled(true);
							btnendMap.setEnabled(true);
							
							MAP=map;
						}
						else
							map = JOptionPane.showInputDialog(frame, "Invalid name, try again:");
					}

				}
			}
		});
		btnendMap.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseClicked(MouseEvent arg0) { // stop mapping
				if(btnsaveMap.isEnabled()){ 
					try {
				        Runtime rt = Runtime.getRuntime();
				        Process pr = rt.exec("ros/end_map.sh");
					} catch (IOException e1) {
						System.out.println("ERROR: fail to launch shell file");
						e1.printStackTrace();
					}
					btnCancelAll.setEnabled(false);
					btnNewButton.setEnabled(true);
					btnLoadMap.setEnabled(false);
					btnLoadMap1.setEnabled(false);
					btnsaveMap.setEnabled(false);
					btnendMap.setEnabled(false);

				}
			}
		});
		
		btnCancelAll.addMouseListener(new MouseAdapter() {
			Runtime rt = Runtime.getRuntime();
			@Override
			public void mouseClicked(MouseEvent e) {  //start or stop moving
				if(btnCancelAll.isEnabled()){
				String map = JOptionPane.showInputDialog(frame, "Map Name:");
				boolean found=false;
				while(!found && map!=null && !map.equals("")){
					File f = new File("/home/or/catkin_ws/src/komodo-iaea-challenge/robotican_komodo/java/IAEA_Challenge/maps/currmap/"+map+".png");
					if(f.exists() && !f.isDirectory()) {
					    MAP = map;
					    found=true;
					    btnCancelAll.setEnabled(false);
						btnNewButton.setEnabled(true);
						btnLoadMap.setEnabled(false);
						btnLoadMap1.setEnabled(false);
						btnsaveMap.setEnabled(false);
						btnendMap.setEnabled(false);
						
						MAP=map; //update current map to last saved
					}
					else
						map = JOptionPane.showInputDialog(frame, "Map not found, try again:");
				}
				}
				
			}
		});
		
		btnNewButton.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseReleased(MouseEvent e) {
				if(btnNewButton.isEnabled() ){
				EventQueue.invokeLater(new Runnable() {
					public void run() {
						try {
							Komodo_Map frame1 = new Komodo_Map(MAP);
							frame1.setVisible(true);
						} catch (Exception e) {
							e.printStackTrace();
						}
					}
				});
				
				btnCancelAll.setEnabled(false);
				btnStartCruise.setEnabled(true);
				}
			}
		});
		
		btnStartCruise.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseClicked(MouseEvent arg0) {
				if(btnStartCruise.isEnabled()){
					FileInteraction.updateMap(MAP);
					try {
				        Runtime rt = Runtime.getRuntime();
				        Process pr = rt.exec("ros/start_mission.sh");
					} catch (IOException e1) {
						System.out.println("ERROR: fail to launch shell file");
						e1.printStackTrace();
					}
					btnNewButton.setEnabled(false);
				}
				
				
			}
		});
		
		frame.setResizable(false);
	}
	
	private Dimension getSmallestScreen(){
		GraphicsEnvironment ge = GraphicsEnvironment.getLocalGraphicsEnvironment();
	    GraphicsDevice[] gs = ge.getScreenDevices();
	    int width=10000,height=10000;
	    System.out.println("-----------------------------");
	    System.out.println("SCREENS LIST:");
	    for (int i = 0; i < gs.length; i++) {
	    	int currW = gs[i].getDisplayMode().getWidth();
	    	int currH = gs[i].getDisplayMode().getHeight();
	        System.out.println(currW+" "+currH);
	        if((currH+currW) < (height+width)){
	        	height = currH;
	        	width = currW;
	        }
	}
	    System.out.println("-----------------------------");
	    return new Dimension(width, height);
	}

}
