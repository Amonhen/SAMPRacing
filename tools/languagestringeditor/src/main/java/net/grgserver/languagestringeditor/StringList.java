package net.grgserver.languagestringeditor;

import javax.swing.JTable;
import javax.swing.event.TableModelEvent;
import javax.swing.event.TableModelListener;
import javax.swing.table.DefaultTableModel;
import javax.swing.table.TableCellRenderer;
import javax.swing.table.TableModel;
import java.awt.Color;
import java.awt.Component;
import java.awt.Rectangle;

public class StringList extends JTable
{
	private boolean changed;

	enum ColumnTypes
	{
		ID,
		FIRST_LANGUAGE,
		LAST_LANGUAGE,
		IGNORE_UNUSED,
		REFERENCES
	}

	public StringList()
	{
		super();

		DefaultTableModel tableModel = new DefaultTableModel()
		{


			@Override
			public boolean isCellEditable(int row, int column)
			{
				// Is a language column
				if (column >= StringList.this.getColumnIndexByType(ColumnTypes.FIRST_LANGUAGE) && column <= StringList.this.getColumnIndexByType(ColumnTypes.LAST_LANGUAGE))
				{
					return true;
				}

				// The "Ignore unused" status can be changed
				if (column == StringList.this.getColumnIndexByType(ColumnTypes.IGNORE_UNUSED))
				{
					return true;
				}

				// Any other column is not editable
				return false;
			}

			@Override
			public Class getColumnClass(int column)
			{
				Object value = this.getValueAt(0, column);
				if (value != null)
				{
					return value.getClass();
				}
				return String.class;
			}
		};
		this.setModel(tableModel);

		TableModelListener tableModelListener = new TableModelListener()
		{
			@Override
			public void tableChanged(TableModelEvent event)
			{
				StringList.this.changed = true;
			}
		};
		tableModel.addTableModelListener(tableModelListener);
	}

	public int getColumnIndexByType(ColumnTypes type)
	{
		switch (type)
		{
			case ID:
				return 0;
			case FIRST_LANGUAGE:
				return 1;
			case LAST_LANGUAGE:
				return this.getColumnCount() - 3;
			case IGNORE_UNUSED:
				return this.getColumnCount() - 2;
			case REFERENCES:
				return this.getColumnCount() - 1;
			default:
				return -1;
		}
	}

	public Component prepareRenderer(TableCellRenderer renderer, int row, int column)
	{
		Component component = super.prepareRenderer(renderer, row, column);

		if (!this.isRowSelected(row))
		{
			TableModel tableModel = this.getModel();

			component.setBackground(this.getBackground());

			int modelRow = this.convertRowIndexToModel(row);

			boolean foundUsed = false;
			for (int languageColumnIndex = 1; languageColumnIndex < tableModel.getColumnCount() - 2; languageColumnIndex++)
			{
				if (((String) tableModel.getValueAt(modelRow, languageColumnIndex)).isEmpty())
				{
					component.setBackground(Color.red);
				}
				else
				{
					foundUsed = true;
				}
			}

			if (!foundUsed)
			{
				component.setBackground(Color.black);
			}
		}

		return component;
	}

	public void scrollToRow(int row)
	{
		this.getSelectionModel().setSelectionInterval(row, row);// Select the row

		this.scrollRectToVisible(new Rectangle(this.getCellRect(row, 0, true)));// Scroll to the row
	}

	public boolean gotoId(int id)
	{
		TableModel tableModel = this.getModel();

		for (int row = 0; row < this.getRowCount(); row++)
		{
			if ((int) tableModel.getValueAt(row, this.getColumnIndexByType(ColumnTypes.ID)) == id)
			{
				this.scrollToRow(row);
				return true;
			}
		}

		return false;
	}

	public boolean searchString(String string)
	{
		int startingRow = this.getSelectedRow() + 1;// Use the next row as the starting row

		// Use the first row if this is already the last one
		if (startingRow >= this.getRowCount())
		{
			startingRow = 0;
		}

		return this.searchString(string, startingRow);
	}

	public boolean searchString(String string, int startingRow)
	{
		TableModel tableModel = this.getModel();

		for (int row = startingRow; row < this.getRowCount(); row++)
		{
			for (int column = this.getColumnIndexByType(ColumnTypes.FIRST_LANGUAGE); column <= this.getColumnIndexByType(ColumnTypes.LAST_LANGUAGE); column++)
			{
				if (((String) tableModel.getValueAt(row, column)).toLowerCase().contains(string.toLowerCase()))
				{
					this.scrollToRow(row);
					return true;
				}
			}
		}

		// Not searched from the start
		if (startingRow != 0)
		{
			return this.searchString(string, 0);// Continue searching from the start
		}

		return false;
	}

	public boolean isChanged()
	{
		return changed;
	}

	public void resetChanged()
	{
		this.changed = false;
	}
}
