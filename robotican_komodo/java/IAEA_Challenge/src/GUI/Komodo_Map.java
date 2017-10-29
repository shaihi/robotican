package GUI;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.EventQueue;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.GraphicsDevice;
import java.awt.GraphicsEnvironment;

import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.border.EmptyBorder;
import javax.swing.plaf.basic.BasicArrowButton;
import javax.swing.ImageIcon;
import java.awt.Image;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.image.BufferedImage;
import java.awt.image.ImageObserver;
import java.io.BufferedWriter;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.Writer;
import java.util.ArrayList;
import javax.swing.JButton;
import javax.swing.JDialog;

import java.awt.List;
import java.awt.RenderingHints;
import java.awt.Toolkit;
import javax.swing.BoxLayout;
import javax.swing.Icon;

import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeEvent;
import java.awt.event.ItemListener;
import java.awt.event.ItemEvent;

public class Komodo_Map extends JFrame {

	private JPanel contentPane;
	private ArrayList<Target> targets = new ArrayList<Target>();
	private List list = new List();

	/**
	 * Launch the application.
	 */
	public static void main(String[] args) {
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				try {
					Komodo_Map frame = new Komodo_Map("currMap");
					frame.setVisible(true);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		});
	}

	/**
	 * Create the frame.
	 */
	public Komodo_Map(String map) {
		Image icon = new ImageIcon(this.getClass().getResource("/robotican.png")).getImage();
		setIconImage(icon);
		setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
		contentPane = new JPanel();
		contentPane.setBorder(new EmptyBorder(0,0,0,0));
		setContentPane(contentPane);
		
		setExtendedState(getExtendedState() | JFrame.MAXIMIZED_BOTH); //make frame full screen
		//setUndecorated(true);
		setVisible(true);
	
		Dimension screenSize = getSmallestScreen(); //get screen size for GUI
		JLabel lblMap = new JLabel("");
		
		
		Image tempImg = new ImageIcon("/home/or/catkin_ws/src/komodo-iaea-challenge/robotican_komodo/java/IAEA_Challenge/maps/currmap/"+map+".png").getImage(); //match map to screen size
		int original_width = tempImg.getWidth(lblMap);
		int original_height = tempImg.getHeight(lblMap);
		double width = (screenSize.getHeight())/(tempImg.getHeight(lblMap))*tempImg.getWidth(lblMap);
		Image img = resizeImage(tempImg,(int) width,(int) screenSize.getHeight());
		int scaled_width = img.getWidth(lblMap);
		int scaled_height = img.getHeight(lblMap);
		float scale = (float) scaled_width/original_width;
		
		System.out.println("original: "+original_width+" , "+original_height);
		System.out.println("scaled: "+scaled_width+" , "+scaled_height);
		System.out.println("scale: "+scale);
		
		lblMap.setIcon(new ImageIcon(img));
		
		
		lblMap.addMouseListener(new MouseAdapter() {
			int x,y,r = 6;
			double angle;
			@Override
			public void mousePressed(MouseEvent e) {

			        //Add this code to draw a circle each time you click.
			        x = e.getX()-(r/2); //Position X (mouse will be in the center of the point)
			        y = e.getY()-(r/2); //Position Y (mouse will be in the center of the point)
			        if(x>0 & y>0 & x<img.getWidth(lblMap) & y<img.getHeight(lblMap)){
			        	Graphics g = getGraphics();
			        	g.setColor(Color.red); //Setting color to red
			        	g.fillOval(x, y, r, r); //Drawing the circle/point
			        	g.drawString(Integer.toString(targets.size()+1), x, y);
			        	g.dispose();
			        	
			        }
			}
			public void mouseReleased(MouseEvent e) {
				if(x>0 & y>0 & x<img.getWidth(lblMap) & y<img.getHeight(lblMap)){
				angle = (Math.atan2( (e.getY()-y) , (e.getX()-x) )); // Desired direction of the robot
				int toX = (int) (Math.round(x + (50 * Math.cos(angle))));
				int toY = (int) (Math.round(y + (50 * Math.sin(angle))));
				Graphics g = getGraphics();
				g.setColor(Color.red);
				g.drawLine(x, y, toX, toY);
				g.dispose();
				
				
				AddTarget1 addTarget = new AddTarget1();
				addTarget.setVisible(true);
				addTarget.setDefaultCloseOperation(JDialog.DISPOSE_ON_CLOSE);
				
				if(addTarget.OK){
					double[] scaling = FileInteraction.getScale("/home/or/catkin_ws/src/komodo-iaea-challenge/robotican_komodo/java/IAEA_Challenge/maps/currmap/"+map+".yaml");
					targets.add(new Target(x, y, angle,addTarget.getDuration(),addTarget.getLift(),addTarget.isUnknownTarget()));
					list.add(new Target(x, y, angle, addTarget.getDuration(),addTarget.getLift(),addTarget.isUnknownTarget()).toString(scaling[0],scaled_height,scaling[1],scaling[2],scale));
					addTarget.setVisible(false);
					addTarget.dispose();
				}
				else{
					targets.add(new Target(x, y, angle, 5,0,false));
					list.add(new Target(x, y, angle, 5,0,false).toString());
					removeTarget(targets.size()-1, img, lblMap);
					addTarget.setVisible(false);
					addTarget.dispose();
				}
				}
			}
		});
		contentPane.setLayout(new BoxLayout(contentPane, BoxLayout.X_AXIS));

		contentPane.add(lblMap);
		
		JPanel panel = new JPanel();
		contentPane.add(panel);
		panel.setLayout(new BoxLayout(panel, BoxLayout.Y_AXIS));
		
		JButton btnDelete = new JButton("      delete      ");
		btnDelete.addMouseListener(new MouseAdapter() {
			int toDelete=-1;
			@Override
			public void mouseClicked(MouseEvent arg0) {
				toDelete = list.getSelectedIndex(); //delete selected target
				removeTarget(toDelete, img, lblMap);
			}
		});
		
		
		JButton btnSelectTargets = new JButton("       Done        ");
		btnSelectTargets.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseClicked(MouseEvent arg0) {
				int use = JOptionPane.showConfirmDialog(lblMap, "Are you sure?", "Use Targets", 0, 3);
				if(use==0){
					String output = "";
					int i=0;
					while(i<targets.size()){
						double[] scaling = FileInteraction.getScale("/home/or/catkin_ws/src/komodo-iaea-challenge/robotican_komodo/java/IAEA_Challenge/maps/currmap/"+map+".yaml");
						output += targets.get(i).toFile(scaling[0],scaled_height,scaling[1],scaling[2],scale)+"\n";
						i++;
					}
					FileInteraction.write("pos.txt", output); //write targets to file

					setVisible(false);
					dispose();
				}
			}
		});
		
		JButton btnNewButton=new JButton("");
		JButton btnMoveDown=new JButton("");
		
		btnNewButton.setIcon(new ImageIcon(this.getClass().getResource("/up1.png")));
		btnMoveDown.setIcon(new ImageIcon(this.getClass().getResource("/down1.png")));
	
		btnNewButton.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseClicked(MouseEvent e) { //move item up
				int i=list.getSelectedIndex();
				swapUp(i,img,lblMap);
			}
		});
		
		btnMoveDown.addMouseListener(new MouseAdapter() { //move item down
			@Override
			public void mouseClicked(MouseEvent e) {
				int i=list.getSelectedIndex();
				swapDown(i,img,lblMap);
			}
		});
		
		
		btnNewButton.setAlignmentX(RIGHT_ALIGNMENT);
		btnMoveDown.setAlignmentX(RIGHT_ALIGNMENT);
		list.addItemListener(new ItemListener() {
			public void itemStateChanged(ItemEvent arg0) {
				int marked = list.getSelectedIndex();
				if(marked!=-1)
					repaintTargets(img, lblMap, marked);
			}
		});
		panel.add(list);
		panel.add(btnDelete);
		panel.add(btnNewButton);
		panel.add(btnMoveDown);
		panel.add(btnSelectTargets);
		panel.setMaximumSize(new Dimension(screenSize.width, screenSize.height-100));
		validate();
		this.setResizable(false);
	}
	
	private boolean isDouble(String s) {
        try {
            Double.parseDouble(s);
            return true;
        } catch (NumberFormatException e) {
            return false;
        }
    }
	
	private void removeTarget(int toDelete, Image img, ImageObserver lblMap){
		if(toDelete!=-1){
			list.remove(toDelete); 
			targets.remove(toDelete);
			repaintTargets(img,lblMap);
	}
}
	
	private void swapUp(int i,Image img, ImageObserver lblMap){ // swap item with the previous index item 
		if(i!=-1 & i!=0){
			String toMove=list.getSelectedItem();
			list.remove(i);
			list.add(toMove, i-1);
			list.select(i-1);
			
			Target temp = targets.get(i);
			targets.set(i, targets.get(i-1));
			targets.set(i-1, temp);
			
			repaintTargets(img, lblMap, list.getSelectedIndex());
		}
		
	}
	
	private void swapDown(int i,Image img, ImageObserver lblMap){ //swap item with the next index item
		if(i!=-1 & i<targets.size()-1){
			String toMove=list.getSelectedItem();
			list.remove(i);
			list.add(toMove, i+1);
			list.select(i+1);
			
			Target temp = targets.get(i);
			targets.set(i, targets.get(i+1));
			targets.set(i+1, temp);
			
			repaintTargets(img, lblMap, list.getSelectedIndex());
		}
	}
	
	private void repaintTargets( Image img, ImageObserver lblMap){
		int i=0;
		Graphics g = getGraphics(); //repaint all targets
		g.drawImage(img, 0, -12, img.getWidth(lblMap), img.getHeight(lblMap), lblMap);
		while(i<targets.size()){
			Target curr = targets.get(i);
			g.setColor(Color.red);
        	g.fillOval(curr.getX(), curr.getY(), 6, 6); 
        	g.drawString(Integer.toString(i+1), curr.getX(), curr.getY());
        	int toX = (int) (Math.round(curr.getX() + (50 * Math.cos(curr.getAngle()))));
			int toY = (int) (Math.round(curr.getY() + (50 * Math.sin(curr.getAngle()))));
			g.drawLine(curr.getX(), curr.getY(), toX, toY);
			i++;
		}
		g.dispose();
	}
	private void repaintTargets( Image img, ImageObserver lblMap, int marked){
		int i=0;
		Graphics g = getGraphics(); //repaint all targets
		g.drawImage(img, 0, -12, img.getWidth(lblMap), img.getHeight(lblMap), lblMap);
		while(i<targets.size()){
			Target curr = targets.get(i);
			
			if(i==marked){
				g.setColor(Color.green);
				g.setFont(g.getFont().deriveFont(20.0f));
			}
			else{
				g.setColor(Color.red);
				g.setFont(g.getFont().deriveFont(12.0f));
			}
			
        	g.fillOval(curr.getX(), curr.getY(), 6, 6); 
        	g.drawString(Integer.toString(i+1), curr.getX(), curr.getY());
        	int toX = (int) (Math.round(curr.getX() + (50 * Math.cos(curr.getAngle()))));
			int toY = (int) (Math.round(curr.getY() + (50 * Math.sin(curr.getAngle()))));
			g.drawLine(curr.getX(), curr.getY(), toX, toY);
			i++;
		}
		g.dispose();
	}
	
	private Image resizeImage(Image srcImg, int w, int h){
	    BufferedImage resizedImg = new BufferedImage(w, h, BufferedImage.TYPE_INT_ARGB);
	    Graphics2D g2 = resizedImg.createGraphics();
	    g2.setRenderingHint(RenderingHints.KEY_INTERPOLATION, RenderingHints.VALUE_INTERPOLATION_BILINEAR);
	    g2.drawImage(srcImg, 0, 0, w, h, null);
	    g2.dispose();
	    return resizedImg;
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
