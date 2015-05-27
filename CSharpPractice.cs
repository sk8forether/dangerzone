using System;
using System.Drawing;
using System.Windows.Forms;

class MForm : Form {

    private Label title = new Label();
    private Label first = new Label();
    private Label last = new Label();
    private Label address = new Label();
    private Label city = new Label();
    private Label hire = new Label();
    private Label position = new Label();
    private Label number = new Label();
    private Label dept = new Label();
    private Label manager = new Label();
    private Label state = new Label();
    
    private TextBox firstbox = new TextBox();
    private TextBox lastbox = new TextBox();
    private TextBox addressbox = new TextBox();
    private TextBox citybox = new TextBox();
    private TextBox hirebox = new TextBox();
    private TextBox positionbox = new TextBox();
    private TextBox numberbox = new TextBox();
    private TextBox deptbox = new TextBox();
    private TextBox managerbox = new TextBox();

    private ComboBox statecb = new ComboBox();

    private Button submit = new Button() {Text = "Submit"};

    private void makeTitle(Label thetitle, int x, int y) {
        Font font = new Font("Times New Roman", 12.0f, FontStyle.Italic | FontStyle.Bold);
        thetitle.Font = font;
        thetitle.Parent = this;
        thetitle.Location = new Point(x, y);
        thetitle.AutoSize = true;
    }
    
    private void makeLabel(Label thingy, int x, int y) {
        Font font = new Font("Times New Roman", 12.0f, FontStyle.Bold);
        thingy.Font = font;
        thingy.Parent = this;
        thingy.Location = new Point(x, y);
        thingy.AutoSize = true;

    }

    private void makeBox(TextBox box, int x, int y) {
        box.Width = 150;
        box.Parent = this;
        box.Location = new Point(x,y);
    }

    private void makeButton(Button press, int x, int y) {
        press.BackColor = Color.Aqua;
        press.AutoSize = false;
        press.Height = 25; press.Width = 100;
        press.Parent = this;
        press.Location = new Point(x,y);
    }

    private void makeCombo(ComboBox cb, int x, int y) {
        Font font = new Font("Times New Roman", 12.0f);
        cb.Font = font;
        cb.Parent = this;
        cb.Location = new Point(x,y);
        cb.Items.AddRange(new object[] {
            "Alabama",
            "Alaska",
            "Arizona",
            "Arkansas",
            "California",
            "Colorado",
            "Connecticut",
            "Delaware",
            "Florida",
            "Georgia",
            "Hawaii",
            "Idaho",
            "Illinois",
            "Indiana",
            "Iowa",
            "Kansas",
            "Kentucky",
            "Louisiana",
            "Maine",
            "Maryland",
            "Massachusetts",
            "Michigan",
            "Minnesota",
            "Mississippi",
            "Missouri",
            "Montana",
            "Nebraska",
            "Nevada",
            "New Hampshire",
            "New Jersey",
            "New Mexico",
            "New York",
            "North Carolina",
            "North Dakota",
            "Ohio",
            "Oklahoma",
            "Oregon",
            "Pennsylvania",
            "Rhode Island",
            "South Carolina",
            "South Dakota",
            "Tennessee",
            "Texas",
            "Utah",
            "Virginia",
            "Vermont",
            "Washington",
            "West Virginia",
            "Wisconsin",
            "Wyoming"});
    }

    public MForm() {
        Text = "Company";
        Size = new Size(700, 400);
        CenterToScreen();

        title.Text = "Employee Information";
        makeTitle(title,250,20);

        //First Column
        first.Text = "First Name:";
        makeLabel(first,20,50);
        makeBox(firstbox,110,50);

        last.Text = "Last Name:";
        makeLabel(last,20,100);
        makeBox(lastbox,110,100);

        address.Text = "Address:";
        makeLabel(address,20,150);
        makeBox(addressbox,110,150);

        city.Text = "City:";
        makeLabel(city,20,200);
        makeBox(citybox,110,200);

        state.Text = "State:";
        makeLabel(state,20,250);
        makeCombo(statecb,110,250);

        //Second Column
        hire.Text = "Hire Date:";
        makeLabel(hire,400,50);
        makeBox(hirebox,490,50);

        position.Text = "Position:";
        makeLabel(position,400,100);
        makeBox(positionbox,490,100);

        number.Text = "Number:";
        makeLabel(number,400,150);
        makeBox(numberbox,490,150);

        dept.Text = "Department:";
        makeLabel(dept,400,200);
        makeBox(deptbox,490,200);

        manager.Text = "Manager:";
        makeLabel(manager,400,250);
        makeBox(managerbox,490,250);

        //Button
        makeButton(submit, 490,300);
        submit.Click += submitClick;
    }


    private void submitClick(object sender, System.EventArgs e)
    {
        if (MessageBox.Show ("You are saving this employee record.  Do you wish to continue?", "You are saving this employee record",
            MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes) {
                Application.Exit();
        }
    }
}


class MApplication {
    public static void Main() {
        Application.Run(new MForm());
    }
}
