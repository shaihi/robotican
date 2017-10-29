package GUI;

import java.awt.EventQueue;

import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.border.EmptyBorder;
import java.awt.GridBagLayout;
import javax.swing.JSpinner;
import javax.swing.JSpinner.DefaultEditor;
import javax.swing.SpinnerModel;
import javax.swing.SpinnerNumberModel;

import java.awt.GridBagConstraints;
import javax.swing.JLabel;
import java.awt.Insets;
import javax.swing.ImageIcon;
import javax.swing.JCheckBox;
import javax.swing.JButton;
import javax.swing.border.TitledBorder;
import javax.swing.BoxLayout;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

public class AddTarget extends JFrame {

	private JPanel contentPane;
	protected double duration;
	protected boolean closeLift;
	protected boolean ok=false;
	protected JButton btnOk, btnCancel;
	private JSpinner spinner;
	private JCheckBox rdbtnLiftOptions;

	/**
	 * Launch the application.
	 */
	public static void main(String[] args) {
		EventQueue.invokeLater(new Runnable() {
			public void run() {
				try {
					AddTarget frame = new AddTarget();
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
	public AddTarget() {
		//setModalExclusionType(null);
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setBounds(100, 100, 400, 150);
		contentPane = new JPanel();
		contentPane.setBorder(new EmptyBorder(5, 5, 5, 5));
		setContentPane(contentPane);
		GridBagLayout gbl_contentPane = new GridBagLayout();
		gbl_contentPane.columnWidths = new int[]{0, 130, 130, 0};
		gbl_contentPane.rowHeights = new int[]{0, 68, 0};
		gbl_contentPane.columnWeights = new double[]{0.0, 0.0, 1.0, Double.MIN_VALUE};
		gbl_contentPane.rowWeights = new double[]{0.0, 0.0, Double.MIN_VALUE};
		contentPane.setLayout(gbl_contentPane);
		
		JLabel label = new JLabel("");
		label.setIcon(new ImageIcon(this.getClass().getResource("/target.png")));
		GridBagConstraints gbc_label = new GridBagConstraints();
		gbc_label.insets = new Insets(0, 0, 5, 5);
		gbc_label.gridx = 0;
		gbc_label.gridy = 0;
		contentPane.add(label, gbc_label);
		
		JPanel panel = new JPanel();
		panel.setBorder(new TitledBorder(null, "Set Duration:", TitledBorder.LEADING, TitledBorder.TOP, null, null));
		GridBagConstraints gbc_panel = new GridBagConstraints();
		gbc_panel.fill = GridBagConstraints.BOTH;
		gbc_panel.insets = new Insets(0, 0, 5, 5);
		gbc_panel.gridx = 1;
		gbc_panel.gridy = 0;
		contentPane.add(panel, gbc_panel);
		panel.setLayout(new BoxLayout(panel, BoxLayout.X_AXIS));
		
		SpinnerModel model = new SpinnerNumberModel(5, 0, 1000, 1); 
		spinner = new JSpinner(model);
		((DefaultEditor) spinner.getEditor()).getTextField().setEditable(false); //make text uneditable
		panel.add(spinner);

		rdbtnLiftOptions = new JCheckBox("close lift");
		rdbtnLiftOptions.setSelected(true);
		GridBagConstraints gbc_rdbtnLiftOptions = new GridBagConstraints();
		gbc_rdbtnLiftOptions.insets = new Insets(0, 0, 5, 0);
		gbc_rdbtnLiftOptions.gridx = 2;
		gbc_rdbtnLiftOptions.gridy = 0;
		contentPane.add(rdbtnLiftOptions, gbc_rdbtnLiftOptions);
		
		btnOk = new JButton("OK");
		
		GridBagConstraints gbc_btnOk = new GridBagConstraints();
		gbc_btnOk.fill = GridBagConstraints.HORIZONTAL;
		gbc_btnOk.anchor = GridBagConstraints.SOUTH;
		gbc_btnOk.insets = new Insets(0, 0, 0, 5);
		gbc_btnOk.gridx = 1;
		gbc_btnOk.gridy = 1;
		contentPane.add(btnOk, gbc_btnOk);
		
		btnCancel = new JButton("      Cancel      ");
		
		GridBagConstraints gbc_btnCancel = new GridBagConstraints();
		gbc_btnCancel.anchor = GridBagConstraints.SOUTH;
		gbc_btnCancel.gridx = 2;
		gbc_btnCancel.gridy = 1;
		contentPane.add(btnCancel, gbc_btnCancel);
		setResizable(false);
	}
	
	public void okClicked(){
		String s = spinner.getValue().toString();
		if(isDouble(s)){
			duration = Double.parseDouble(s);
			closeLift = rdbtnLiftOptions.isSelected();
			ok=true;
		}
		else
			ok=false;
		
		dispose();
	}
	
	private boolean isDouble(String s) {
        try {
            Double.parseDouble(s);
            return true;
        } catch (NumberFormatException e) {
            return false;
        }
    }
	
}
