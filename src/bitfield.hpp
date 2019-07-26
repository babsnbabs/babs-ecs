#pragma once

// The bitfield namespace contains a type and operations with a more intuitive naming convention.
// It's up to the caller to keep track of which flags are being used and to increase the flag index
// by 2 each time a new flag is needed.
namespace bitfield
{
	typedef uint32_t Bitfield;
	
    /**
     * Set the flag(s) on the field.
     * 
     * example:
     *  field   0001
     *  flag    0010
     *  ------------
     *  returns 0011
     */
	Bitfield Set(Bitfield field, Bitfield flag)
	{
		return field | flag;
	}

    /**
     * Clear the flag(s) on the field.
     * 
     * example:
     *  field   0011
     *  flag    0010
     *  ------------
     *  returns 0001
     */
	Bitfield Clear(Bitfield field, Bitfield flag)
	{
		return field ^ flag;
	}

    /**
     * Checks if the flag(s) exist in the field.
     * 
     * example:
     *  field   0001    0001    0101    0111
     *  flag    0010    0001    0011    0011
     *  ------------------------------------
     *          0000    0001    0001    0011
     * 
     *  returns false   true    false   true
     */
	bool Has(Bitfield field, Bitfield flag)
	{
		return (field & flag) == flag;
	}
};
