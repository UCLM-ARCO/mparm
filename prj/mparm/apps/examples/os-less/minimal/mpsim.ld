/* ld script to make ARM Linux kernel
 * taken from the i386 version by Russell King
 * Written by Martin Mares <mj@atrey.karlin.mff.cuni.cz>
 */
OUTPUT_ARCH(arm)
SECTIONS {
	. = 0;
	.vectors1 : {
	        *(.resetvector1)
	}

	/DISCARD/ : {			/* Exit code and data		*/
		*(.text.exit)
		*(.data.exit)
		*(.exitcall.exit)
	}

        . = 0x8000;
	.text : {
		_text = .;
			*(.text.init)
			*(.text)
		_etext = .;
	}

	. = ALIGN(8192);
	.data : {
		*(.init.task)
		. = ALIGN(32);
		*(.data)
		CONSTRUCTORS
		_edata = .;
	}

	.bss : {
		__bss_start = .;
		*(.bss)
		*(COMMON)
		__bss_end = . ;
	}
        
        . = 0x400000;
	.stack : {
	        __stack_start = . ;
	}

	. = ALIGN(8192);
	_end_kernel = . ;
}
