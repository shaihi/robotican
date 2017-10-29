package GUI;

import java.awt.BorderLayout;
import java.awt.FlowLayout;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JPanel;
import javax.swing.JSpinner;
import javax.swing.SpinnerModel;
import javax.swing.SpinnerNumberModel;
import javax.swing.JSpinner.DefaultEditor;
import javax.swing.border.EmptyBorder;
import javax.swing.border.TitledBorder;
import java.awt.GridBagLayout;
import javax.swing.JLabel;
import java.awt.GridBagConstraints;
import java.awt.Insets;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import javax.swing.JCheckBox;

public class AddTarget1 extends JDialog{

	private final JPanel contentPanel = new JPanel();
	private JSpinner duration,lift;
	private JCheckBox unknownTarget;
	protected boolean OK=false;

	/**
	 * Launch the application.
	 */
	public static void main(String[] args) {
		try {
			AddTarget1 dialog = new AddTarget1();
			dialog.setDefaultCloseOperation(JDialog.DISPOSE_ON_CLOSE);
			dialog.setVisible(true);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	/**
	 * Create the dialog.
	 */
	public AddTarget1() {
		
		setBounds(100, 100, 570, 120);
		getContentPane().setLayout(new BorderLayout());
		setModal(true);
		setResizable(false);
		contentPanel.setBorder(new EmptyBorder(5, 5, 5, 5));
		getContentPane().add(contentPanel, BorderLayout.CENTER);
		GridBagLayout gbl_contentPanel = new GridBagLayout();
		gbl_contentPanel.columnWidths = new int[]{0, 120, 0, 0, 0};
		gbl_contentPanel.rowHeights = new int[]{86, 0};
		gbl_contentPanel.columnWeights = new double[]{0.0, 1.0, 1.0, 0.0, Double.MIN_VALUE};
		gbl_contentPanel.rowWeights = new double[]{1.0, Double.MIN_VALUE};
		contentPanel.setLayout(gbl_contentPanel);
		{
			JLabel label = new JLabel("");
			label.setIcon(new ImageIcon(this.getClass().getResource("/target.png")));
			GridBagConstraints gbc_label = new GridBagConstraints();
			gbc_label.insets = new Insets(0, 0, 0, 5);
			gbc_label.gridx = 0;
			gbc_label.gridy = 0;
			contentPanel.add(label, gbc_label);
		}
		{
			JPanel panel = new JPanel();
			panel.setBorder(new TitledBorder(null, "Set Duration:", TitledBorder.LEADING, TitledBorder.TOP, null, null));
			GridBagConstraints gbc_panel = new GridBagConstraints();
			gbc_panel.insets = new Insets(0, 0, 0, 5);
			gbc_panel.fill = GridBagConstraints.BOTH;
			gbc_panel.gridx = 1;
			gbc_panel.gridy = 0;
			contentPanel.add(panel, gbc_panel);
			
			SpinnerModel model = new SpinnerNumberModel(5, 0, 1000, 1); 
			duration = new JSpinner(model);
			((DefaultEditor) duration.getEditor()).getTextField().setEditable(false); //make text uneditable
			panel.add(duration);
		}
		{
			JPanel panel = new JPanel();
			panel.setBorder(new TitledBorder(null, "Set Lift Elevation:", TitledBorder.LEADING, TitledBorder.TOP, null, null));
			GridBagConstraints gbc_panel = new GridBagConstraints();
			gbc_panel.insets = new Insets(0, 0, 0, 5);
			gbc_panel.fill = GridBagConstraints.BOTH;
			gbc_panel.gridx = 2;
			gbc_panel.gridy = 0;
			contentPanel.add(panel, gbc_panel);
			
			SpinnerModel model = new SpinnerNumberModel(0, 0, 100, 1); 
			lift = new JSpinner(model);
			((DefaultEditor) lift.getEditor()).getTextField().setEditable(false); //make text uneditable
			panel.add(lift);
		}
		{
			unknownTarget = new JCheckBox("unknown target?");
			GridBagConstraints gbc_chckbxCheck = new GridBagConstraints();
			gbc_chckbxCheck.gridx = 3;
			gbc_chckbxCheck.gridy = 0;
			contentPanel.add(unknownTarget, gbc_chckbxCheck);
		}
		{
			JPanel buttonPane = new JPanel();
			buttonPane.setLayout(new FlowLayout(FlowLayout.RIGHT));
			getContentPane().add(buttonPane, BorderLayout.SOUTH);
			{
				JButton okButton = new JButton("OK");
				okButton.addMouseListener(new MouseAdapter() {
					@Override
					public void mouseClicked(MouseEvent arg0) {
						OK=true;
						setVisible(false);
						dispose();
					}
				});
				//okButton.setActionCommand("OK");
				buttonPane.add(okButton);
				getRootPane().setDefaultButton(okButton);
			}
			{
				JButton cancelButton = new JButton("Cancel");
				cancelButton.addMouseListener(new MouseAdapter() {
					@Override
					public void mouseClicked(MouseEvent e) {
						setVisible(false);
						dispose();
					}
				});
				//cancelButton.setActionCommand("Cancel");
				buttonPane.add(cancelButton);
			}
		}

		
	}
	
	public double getDuration(){
		int output = (int) duration.getValue();
		return (double)output;
	}
	public double getLift(){
		int output = (int) lift.getValue();
		return (double)output;
	}
	public boolean isUnknownTarget(){
		return unknownTarget.isSelected();
	}
	

}